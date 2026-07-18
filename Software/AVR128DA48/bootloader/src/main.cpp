/*
 *  MIT License
 *
 *  Copyright (c) 2021-2026 DigitalConfections
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

/*
 * FlexFox80 AVR128DA48 resident serial bootloader, adapted from SignalSlinger.
 */
#include "bootloader_config.h"
#include "boot_update_session.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <ccp.h>

#ifndef USART_CHSIZE0_bm
#define USART_CHSIZE0_bm USART_CHSIZE_0_bm
#define USART_CHSIZE1_bm USART_CHSIZE_1_bm
#endif

#define USART1_BAUD_VALUE(baud) ((uint16_t)((F_CPU * 64UL / (16UL * (baud))) + 0.5))
#define WIFI_ENABLE_PIN PIN5_bm
#define WIFI_RESET_PIN PIN6_bm
#define V3V3_POWER_ENABLE_PIN PIN7_bm
#define MAIN_POWER_ENABLE_PIN PIN4_bm
#define SWITCH_PIN PIN7_bm
#define RED_LED_PIN PIN6_bm
#define GREEN_LED_PIN PIN5_bm
#define LED_PINS (RED_LED_PIN | GREEN_LED_PIN)
#define UPDATE_LED_HALF_PERIOD_TICKS 25000U
#define USART_RX_ERROR_MASK (USART_PERR_bm | USART_FERR_bm | USART_BUFOVF_bm)
#define RESET_FLAGS_MASK (RSTCTRL_UPDIRF_bm | RSTCTRL_SWRF_bm | RSTCTRL_WDRF_bm | RSTCTRL_EXTRF_bm | RSTCTRL_BORF_bm | RSTCTRL_PORF_bm)

static uint8_t page_buffer[FLEXFOX_FLASH_PAGE_BYTES];
static uint8_t last_nvm_error;
static uint8_t last_usart_error;
static uint16_t update_led_ticks;
static bool update_led_error;
static bool update_recovery_required;
static uint8_t boot_reset_flags;
static BootUpdateSession update_session;

static uint8_t read_and_clear_reset_flags(void)
{
	uint8_t flags = RSTCTRL.RSTFR;
	RSTCTRL.RSTFR = RESET_FLAGS_MASK;
	return flags;
}

static bool app_requested_bootloader(uint8_t reset_flags)
{
	bool requested = ((reset_flags & RSTCTRL_SWRF_bm) != 0U) &&
	                 (GPR.GPR1 == FLEXFOX_BOOT_APP_UPDATE_REQUEST);
	GPR.GPR1 = 0U;
	return requested;
}

static bool persistent_update_requested(void)
{
	while(NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm) { }
	return *(const volatile uint8_t *)(MAPPED_EEPROM_START + FLEXFOX_AVR_UPDATE_EEPROM_MARKER_ADDRESS) ==
	       FLEXFOX_AVR_UPDATE_EEPROM_MARKER;
}

static bool clear_persistent_update_request(void)
{
	while(NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm) { }
	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);
	*(volatile uint8_t *)(MAPPED_EEPROM_START + FLEXFOX_AVR_UPDATE_EEPROM_MARKER_ADDRESS) = 0xffU;
	while(NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm) { }
	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
	return *(const volatile uint8_t *)(MAPPED_EEPROM_START + FLEXFOX_AVR_UPDATE_EEPROM_MARKER_ADDRESS) == 0xffU;
}

static void clock_init(void)
{
	ccp_write_io((void *)&CLKCTRL.OSCHFCTRLA,
	             CLKCTRL_FRQSEL_24M_gc | (0 << CLKCTRL_AUTOTUNE_bp) | (0 << CLKCTRL_RUNSTDBY_bp));
}

static void pins_init(bool preserve_wifi_session)
{
	/*
	 * Set the output latches before enabling the drivers.  During an authorized
	 * update the ESP is already running the updater, so keep both of its power
	 * enables and its reset line high across the AVR software-reset handoff.
	 * A normal boot retains the conservative cold-start sequence: rails off,
	 * regulator on, WiFi enable on, then reset released after each stage settles.
	 */
	PORTB.OUTSET = MAIN_POWER_ENABLE_PIN;
	if(preserve_wifi_session)
		PORTA.OUTSET = V3V3_POWER_ENABLE_PIN | WIFI_ENABLE_PIN | WIFI_RESET_PIN;
	else
		PORTA.OUTCLR = V3V3_POWER_ENABLE_PIN | WIFI_ENABLE_PIN | WIFI_RESET_PIN;
	PORTA.DIRSET = V3V3_POWER_ENABLE_PIN | WIFI_ENABLE_PIN | WIFI_RESET_PIN;
	PORTB.DIRSET = MAIN_POWER_ENABLE_PIN;
	if(!preserve_wifi_session)
	{
		PORTA.OUTSET = V3V3_POWER_ENABLE_PIN;
		_delay_ms(100);
		PORTA.OUTSET = WIFI_ENABLE_PIN;
		_delay_ms(100);
		PORTA.OUTSET = WIFI_RESET_PIN;
	}

	PORTC.OUTSET = LED_PINS;
	PORTC.DIRSET = LED_PINS;
	PORTC.DIRCLR = SWITCH_PIN;
	PORTC.PIN7CTRL = PORT_PULLUPEN_bm;

	/* USART1 on PC0 TX and PC1 RX. */
	PORTC.DIRSET = PIN0_bm;
	PORTC.OUTSET = PIN0_bm;
	PORTC.DIRCLR = PIN1_bm;
	PORTC.PIN1CTRL = 0;
}

static bool switch_is_held(void) { return (PORTC.IN & SWITCH_PIN) == 0U; }
static void leds_off(void) { PORTC.OUTCLR = LED_PINS; }
static void leds_toggle(void) { PORTC.OUTTGL = LED_PINS; }
static void update_led_start(void)
{
	/* Alternating colors are reserved for resident-bootloader work. Start from
	 * a deterministic phase so every wait and recovery interval remains visible. */
	PORTC.OUTSET = RED_LED_PIN;
	PORTC.OUTCLR = GREEN_LED_PIN;
	update_led_ticks = 0U;
	update_led_error = false;
}

/* Called after a ten-microsecond delay in every bootloader wait loop. This
 * keeps the progress indication alive during the ESP's long state-save gaps
 * without enabling interrupts while Flash is being changed. */
static void update_led_service_10us(void)
{
	if(update_led_error) return;
	if(++update_led_ticks >= UPDATE_LED_HALF_PERIOD_TICKS)
	{
		update_led_ticks = 0U;
		PORTC.OUTTGL = LED_PINS;
	}
}

static void error_led(void)
{
	update_led_error = true;
	PORTC.OUTSET = RED_LED_PIN;
	PORTC.OUTCLR = GREEN_LED_PIN;
}

static void usart_init(void)
{
	USART1.BAUD = USART1_BAUD_VALUE(FLEXFOX_BOOT_USART_BAUD);
	USART1.CTRLA = 0;
	USART1.CTRLC = USART_CHSIZE0_bm | USART_CHSIZE1_bm;
	USART1.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
}

static bool usart_rx_ready(void) { return (USART1.STATUS & USART_RXCIF_bm) != 0U; }

static bool usart_read_byte(uint8_t *value)
{
	uint8_t status = USART1.RXDATAH;
	*value = USART1.RXDATAL;
	last_usart_error = status & USART_RX_ERROR_MASK;
	return last_usart_error == 0U;
}

static bool usart_read_timeout(uint8_t *value, uint16_t timeout_ms)
{
	uint32_t ticks = (uint32_t)timeout_ms * 100U;
	last_usart_error = 0;
	while(ticks--)
	{
		if(usart_rx_ready()) return usart_read_byte(value);
		_delay_us(10);
		update_led_service_10us();
	}
	return false;
}

static void usart_flush_rx(void)
{
	uint8_t ignored;
	while(usart_rx_ready()) (void)usart_read_byte(&ignored);
	last_usart_error = 0;
}

static void usart_write(char value)
{
	while((USART1.STATUS & USART_DREIF_bm) == 0U) {}
	USART1.TXDATAL = value;
}

static void usart_write_text(const char *text) { while(*text) usart_write(*text++); }
static void hex_nibble(uint8_t value) { value &= 0x0FU; usart_write((char)(value < 10U ? '0' + value : 'A' + value - 10U)); }
static void hex8(uint8_t value) { hex_nibble(value >> 4); hex_nibble(value); }
static void hex16(uint16_t value) { hex8((uint8_t)(value >> 8)); hex8((uint8_t)value); }
static void hex32(uint32_t value) { hex16((uint16_t)(value >> 16)); hex16((uint16_t)value); }
static void send_ok(const char *detail) { usart_write_text("OK "); usart_write_text(detail); usart_write_text("\r\n"); }
static void send_error(const char *detail) { usart_write_text("ERR "); usart_write_text(detail); usart_write_text("\r\n"); error_led(); }

static uint16_t crc16_update(uint16_t crc, uint8_t value)
{
	crc ^= (uint16_t)value << 8;
	for(uint8_t bit = 0; bit < 8; bit++) crc = (crc & 0x8000U) ? (uint16_t)((crc << 1) ^ 0x1021U) : (uint16_t)(crc << 1);
	return crc;
}

static uint32_t crc32_update(uint32_t crc, uint8_t value)
{
	crc ^= value;
	for(uint8_t bit = 0; bit < 8U; bit++)
	{
		crc = (crc >> 1) ^ ((crc & 1U) ? 0xedb88320UL : 0U);
	}
	return crc;
}

static bool read_crc_byte(uint8_t *value, uint16_t *crc)
{
	if(!usart_read_timeout(value, FLEXFOX_BOOT_FRAME_BYTE_TIMEOUT_MS)) return false;
	*crc = crc16_update(*crc, *value);
	return true;
}

static bool read_u32_le(uint32_t *value, uint16_t *crc)
{
	*value = 0;
	for(uint8_t shift = 0; shift < 32; shift += 8)
	{
		uint8_t byte;
		if(!read_crc_byte(&byte, crc)) return false;
		*value |= (uint32_t)byte << shift;
	}
	return true;
}

static bool read_expected_crc(uint16_t *value)
{
	uint8_t low, high;
	if(!usart_read_timeout(&low, FLEXFOX_BOOT_FRAME_BYTE_TIMEOUT_MS) ||
	   !usart_read_timeout(&high, FLEXFOX_BOOT_FRAME_BYTE_TIMEOUT_MS)) return false;
	*value = (uint16_t)low | ((uint16_t)high << 8);
	return true;
}

static bool read_page_payload(uint16_t *crc)
{
	for(uint16_t index = 0; index < FLEXFOX_FLASH_PAGE_BYTES; index++)
	{
		if(!read_crc_byte(&page_buffer[index], crc)) return false;
	}
	return true;
}

static bool page_is_writable(uint32_t address)
{
	return (address % FLEXFOX_FLASH_PAGE_BYTES) == 0U &&
	       address >= FLEXFOX_APP_START_BYTES &&
	       address <= FLEXFOX_FLASH_BYTES - FLEXFOX_FLASH_PAGE_BYTES;
}

static uint16_t buffer_crc(void)
{
	uint16_t crc = 0xFFFFU;
	for(uint16_t index = 0; index < FLEXFOX_FLASH_PAGE_BYTES; index++)
	{
		crc = crc16_update(crc, page_buffer[index]);
	}
	return crc;
}

static void nvm_wait(void) { while(NVMCTRL.STATUS & NVMCTRL_FBUSY_bm) {} }
static void nvm_command(NVMCTRL_CMD_t command) { ccp_write_spm((void *)&NVMCTRL.CTRLA, command); }

static void pgm_word_write(uint32_t byte_address, uint16_t value)
{
	asm volatile(
	    "movw r30, %A0\n\t"
	    "sts %1, %C0\n\t"
	    "movw r0, %2\n\t"
	    "spm\n\t"
	    "clr r1\n\t"
	    : : "r"(byte_address), "i"(_SFR_MEM_ADDR(RAMPZ)), "r"(value)
	    : "r0", "r30", "r31", "memory");
}

static bool erase_page(uint32_t address)
{
	if(!page_is_writable(address)) return false;
	nvm_wait(); nvm_command(NVMCTRL_CMD_NONE_gc); nvm_command(NVMCTRL_CMD_FLPER_gc);
	pgm_word_write(address, 0x0000U); nvm_wait();
	last_nvm_error = NVMCTRL.STATUS & NVMCTRL_ERROR_gm;
	nvm_command(NVMCTRL_CMD_NONE_gc);
	return last_nvm_error == NVMCTRL_ERROR_NOERROR_gc;
}

static bool write_page(uint32_t address)
{
	if(!page_is_writable(address)) return false;
	nvm_wait(); nvm_command(NVMCTRL_CMD_NONE_gc); nvm_command(NVMCTRL_CMD_FLWR_gc);
	for(uint16_t index = 0; index < FLEXFOX_FLASH_PAGE_BYTES; index += 2)
	{
		uint16_t word = (uint16_t)page_buffer[index] | ((uint16_t)page_buffer[index + 1] << 8);
		pgm_word_write(address + index, word);
	}
	nvm_wait(); last_nvm_error = NVMCTRL.STATUS & NVMCTRL_ERROR_gm;
	nvm_command(NVMCTRL_CMD_NONE_gc);
	return last_nvm_error == NVMCTRL_ERROR_NOERROR_gc;
}

static uint16_t page_crc(uint32_t address)
{
	uint16_t crc = 0xFFFFU;
	nvm_wait();
	for(uint16_t index = 0; index < FLEXFOX_FLASH_PAGE_BYTES; index++) crc = crc16_update(crc, pgm_read_byte_far(address + index));
	return crc;
}

static uint16_t flash_u16_le(uint32_t address)
{
	return (uint16_t)pgm_read_byte_far(address) |
	       ((uint16_t)pgm_read_byte_far(address + 1U) << 8);
}

static uint32_t flash_u32_le(uint32_t address)
{
	uint32_t value = 0U;
	for(uint8_t offset = 0; offset < 4U; offset++)
	{
		value |= (uint32_t)pgm_read_byte_far(address + offset) << (offset * 8U);
	}
	return value;
}

static uint32_t candidate_crc32(uint32_t byte_count)
{
	uint32_t crc = 0xffffffffUL;
	for(uint16_t index = 0; index < FLEXFOX_FLASH_PAGE_BYTES && index < byte_count; index++)
	{
		crc = crc32_update(crc, page_buffer[index]);
	}
	for(uint32_t offset = FLEXFOX_FLASH_PAGE_BYTES; offset < byte_count; offset++)
	{
		crc = crc32_update(crc, pgm_read_byte_far(FLEXFOX_APP_START_BYTES + offset));
	}
	return crc ^ 0xffffffffUL;
}

static bool candidate_image_is_valid(void)
{
	if(!bootUpdateSessionReadyForResetWrite(&update_session)) return false;
	const uint32_t payload_bytes = update_session.image_bytes - FLEXFOX_FLASH_PAGE_BYTES;
	const uint32_t trailer_address = FLEXFOX_APP_START_BYTES + payload_bytes;
	const char expected_magic[] = FLEXFOX_AVR_UPDATE_TRAILER_MAGIC;
	for(uint8_t index = 0; index < FLEXFOX_AVR_UPDATE_TRAILER_MAGIC_BYTES; index++)
	{
		if(pgm_read_byte_far(trailer_address + index) != (uint8_t)expected_magic[index]) return false;
	}
	if(flash_u16_le(trailer_address + FLEXFOX_AVR_UPDATE_TRAILER_FORMAT_OFFSET) !=
	     FLEXFOX_AVR_UPDATE_IMAGE_FORMAT_VERSION ||
	   flash_u16_le(trailer_address + FLEXFOX_AVR_UPDATE_TRAILER_PAGE_SIZE_OFFSET) !=
	     FLEXFOX_FLASH_PAGE_BYTES ||
	   flash_u32_le(trailer_address + FLEXFOX_AVR_UPDATE_TRAILER_APP_START_OFFSET) !=
	     FLEXFOX_APP_START_BYTES ||
	   flash_u32_le(trailer_address + FLEXFOX_AVR_UPDATE_TRAILER_PAYLOAD_BYTES_OFFSET) !=
	     payload_bytes)
	{
		return false;
	}
	if(candidate_crc32(payload_bytes) !=
	   flash_u32_le(trailer_address + FLEXFOX_AVR_UPDATE_TRAILER_PAYLOAD_CRC32_OFFSET))
	{
		return false;
	}
	return candidate_crc32(update_session.image_bytes) == update_session.image_crc32;
}

static bool receive_address(char command, uint32_t *address, bool with_payload)
{
	uint16_t crc = crc16_update(0xFFFFU, (uint8_t)command), expected;
	if(!read_u32_le(address, &crc) || (with_payload && !read_page_payload(&crc)) || !read_expected_crc(&expected))
	{
		if(last_usart_error) { usart_write_text("ERR serial "); hex8(last_usart_error); usart_write_text("\r\n"); }
		else send_error("timeout");
		return false;
	}
	if(crc != expected) { send_error("crc"); return false; }
	if(!page_is_writable(*address)) { send_error("address"); return false; }
	return true;
}

static void handle_begin(char command)
{
	uint16_t crc = crc16_update(0xFFFFU, (uint8_t)command), expected;
	uint32_t image_bytes, image_crc32;
	if(!read_u32_le(&image_bytes, &crc) || !read_u32_le(&image_crc32, &crc) ||
	   !read_expected_crc(&expected))
	{
		if(last_usart_error) { usart_write_text("ERR serial "); hex8(last_usart_error); usart_write_text("\r\n"); error_led(); }
		else send_error("timeout");
		return;
	}
	if(crc != expected) { send_error("crc"); return; }
	if(!bootUpdateSessionBegin(&update_session, image_bytes, image_crc32)) { send_error("image"); return; }
	update_recovery_required = true;
	send_ok("begin");
}

static void handle_erase(char command)
{
	uint32_t address;
	if(!receive_address(command, &address, false)) return;
	if(!bootUpdateSessionAllowsErase(&update_session, address)) { send_error("sequence"); return; }
	if(erase_page(address))
	{
		if(address == FLEXFOX_APP_START_BYTES && !clear_persistent_update_request()) { send_error("marker"); return; }
		bootUpdateSessionNoteErase(&update_session, address);
		send_ok("erase");
	}
	else { usart_write_text("ERR nvm "); hex8(last_nvm_error); usart_write_text("\r\n"); error_led(); }
}

static void handle_write(char command)
{
	uint32_t address;
	if(!receive_address(command, &address, true)) return;
	const uint16_t expected_page_crc = buffer_crc();
	if(!bootUpdateSessionAllowsWrite(&update_session, address, expected_page_crc)) { send_error("sequence"); return; }
	if(address == FLEXFOX_APP_START_BYTES && !update_session.committed && !candidate_image_is_valid())
	{
		send_error("image");
		return;
	}
	if(!write_page(address))
	{
		usart_write_text("ERR nvm "); hex8(last_nvm_error); usart_write_text("\r\n"); error_led();
		return;
	}
	if(page_crc(address) != expected_page_crc) { send_error("verify"); return; }
	bootUpdateSessionNoteWrite(&update_session, address, expected_page_crc);
	send_ok("write");
}

static void handle_crc(char command)
{
	uint32_t address;
	if(!receive_address(command, &address, false)) return;
	usart_write_text("OK crc 0x"); hex32(address); usart_write(' '); hex16(page_crc(address)); usart_write_text("\r\n");
}

static bool app_looks_programmed(void) { return pgm_read_word_far(FLEXFOX_APP_START_BYTES) != 0xFFFFU; }

static void jump_to_application(void)
{
	cli(); USART1.CTRLB = 0; USART1.CTRLA = 0;
	GPR.GPR1 = FLEXFOX_BOOT_HANDOFF_INFO_MAGIC | (FLEXFOX_BOOT_PROTOCOL_VERSION & FLEXFOX_BOOT_HANDOFF_INFO_PROTOCOL_MASK);
	GPR.GPR2 = FLEXFOX_BOOTLOADER_VERSION_MAJOR;
	GPR.GPR3 = FLEXFOX_BOOTLOADER_VERSION_MINOR;
	asm volatile("jmp 0x4000");
}

static void send_banner(void)
{
	usart_write_text("\r\nFlexFox80 "); usart_write_text(FLEXFOX_BOOTLOADER_VERSION);
	usart_write_text(" proto=2 minproto=2 maxproto=2 app=0x4000 page=512 flash=131072 baud=");
	usart_write_text(FLEXFOX_BOOT_USART_BAUD_TEXT);
	usart_write_text(" boot=32 write=0x4000-0x1FFFF features=appmark,pagecrc,sessioncrc,producttrailer,resetlast,diagnostic,esppower cmds=U,R,?,D,B,E,W,C\r\n");
}

static void send_diagnostic(void)
{
	usart_write_text("OK diag reset="); hex8(boot_reset_flags);
	usart_write_text(" recovery="); usart_write(update_recovery_required ? '1' : '0');
	usart_write_text(" active="); usart_write(update_session.active ? '1' : '0');
	usart_write_text(" reset_erased="); usart_write(update_session.reset_page_erased ? '1' : '0');
	usart_write_text(" committed="); usart_write(update_session.committed ? '1' : '0');
	usart_write_text(" next="); hex16(update_session.next_page);
	usart_write('/'); hex16(update_session.page_count);
	usart_write_text(" bytes="); hex32(update_session.image_bytes);
	usart_write_text(" crc="); hex32(update_session.image_crc32);
	usart_write_text("\r\n");
}

static bool serial_entry_requested(void)
{
	static const uint8_t entry_suffix[] = {'B', 'L', '2', '!'};
	for(uint16_t elapsed = 0; elapsed < FLEXFOX_BOOT_ENTRY_WINDOW_MS; elapsed++)
	{
		if(usart_rx_ready())
		{
			uint8_t byte;
			if(usart_read_byte(&byte))
			{
				if(byte == FLEXFOX_UPDATE_REQUEST_CHAR)
				{
					uint16_t crc = crc16_update(0xFFFFU, byte), expected;
					bool valid = true;
					for(uint8_t index = 0; index < sizeof(entry_suffix); index++)
					{
						uint8_t received;
						if(!usart_read_timeout(&received, 250U) || received != entry_suffix[index])
						{
							valid = false;
							break;
						}
						crc = crc16_update(crc, received);
					}
					if(valid && read_expected_crc(&expected) && crc == expected)
					{
						usart_write_text("BOOT\r\n");
						return true;
					}
				}
				if(byte == FLEXFOX_INFO_CHAR) send_banner();
				if(byte == FLEXFOX_RUN_APP_CHAR) return false;
			}
		}
		if(switch_is_held() && (elapsed % 50U) == 0U) leds_toggle();
		_delay_ms(1);
	}
	return false;
}

int main(void)
{
	cli();
	uint8_t reset_flags = read_and_clear_reset_flags();
	boot_reset_flags = reset_flags;
	bool power_start = (reset_flags & (RSTCTRL_PORF_bm | RSTCTRL_BORF_bm)) != 0U;
	bool app_request = app_requested_bootloader(reset_flags);
	bool persistent_request = persistent_update_requested();
	bool requested = app_request || persistent_request;
	/* Preserve a live ESP only across the deliberate software-reset handoff.
	 * A persistent marker after POR/BOR means the ESP is cold and needs the
	 * normal rail/reset sequencing before it can resume recovery. */
	clock_init(); pins_init(bootUpdatePreserveLiveEsp(app_request, power_start)); usart_init();
	bootUpdateSessionReset(&update_session);
	update_recovery_required = requested;
	bool stay = requested || (!power_start && switch_is_held()) || !app_looks_programmed() || serial_entry_requested();
	if(!stay) { leds_off(); jump_to_application(); }

	update_led_start();
	usart_flush_rx(); send_banner(); usart_write_text("Waiting for updater\r\n");
	for(;;)
	{
		if(!usart_rx_ready())
		{
			_delay_us(10);
			update_led_service_10us();
			continue;
		}
		uint8_t byte;
		if(!usart_read_byte(&byte)) { send_error("serial"); continue; }
		/* Only a command that can advance/retry programming clears a latched
		 * error. Read-only banner/diagnostic queries preserve the error signal. */
		if(byte == FLEXFOX_UPDATE_REQUEST_CHAR || byte == FLEXFOX_BEGIN_UPDATE_CHAR ||
		   byte == FLEXFOX_ERASE_PAGE_CHAR || byte == FLEXFOX_WRITE_PAGE_CHAR ||
		   byte == FLEXFOX_CRC_PAGE_CHAR)
		{
			update_led_start();
		}
		if(byte == FLEXFOX_INFO_CHAR) send_banner();
		else if(byte == FLEXFOX_DIAGNOSTIC_CHAR) send_diagnostic();
		else if(byte == FLEXFOX_UPDATE_REQUEST_CHAR) usart_write_text("BOOT\r\n");
		else if(byte == FLEXFOX_RUN_APP_CHAR &&
		        bootUpdateSessionMayRunApplication(&update_session, update_recovery_required, app_looks_programmed()))
		{
			jump_to_application();
		}
		else if(byte == FLEXFOX_BEGIN_UPDATE_CHAR) handle_begin((char)byte);
		else if(byte == FLEXFOX_ERASE_PAGE_CHAR) handle_erase((char)byte);
		else if(byte == FLEXFOX_WRITE_PAGE_CHAR) handle_write((char)byte);
		else if(byte == FLEXFOX_CRC_PAGE_CHAR) handle_crc((char)byte);
		else send_error("unsupported");
	}
}
