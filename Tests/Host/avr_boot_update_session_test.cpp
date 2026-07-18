#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "boot_update_session.h"

namespace {

constexpr std::uint32_t imageBytes = 4U * FLEXFOX_FLASH_PAGE_BYTES;
constexpr std::uint32_t imageCrc32 = 0x12345678UL;
constexpr std::uint16_t page1Crc = 0x1111U;
constexpr std::uint16_t page2Crc = 0x2222U;
constexpr std::uint16_t trailerCrc = 0x3333U;
constexpr std::uint16_t resetCrc = 0x4444U;

void expect(bool condition, const char *name)
{
	if(!condition)
	{
		std::cerr << "FAIL " << name << '\n';
		std::exit(EXIT_FAILURE);
	}
	std::cout << "PASS " << name << '\n';
}

std::uint32_t pageAddress(std::uint16_t page)
{
	return FLEXFOX_APP_START_BYTES + (std::uint32_t)page * FLEXFOX_FLASH_PAGE_BYTES;
}

BootUpdateSession begunSession()
{
	BootUpdateSession session;
	bootUpdateSessionReset(&session);
	expect(bootUpdateSessionBegin(&session, imageBytes, imageCrc32), "valid_session_begins");
	return session;
}

void invalid_images_fail_closed()
{
	BootUpdateSession session;
	bootUpdateSessionReset(&session);
	expect(!bootUpdateSessionBegin(&session, 0, imageCrc32), "empty_image_is_rejected");
	expect(
		!bootUpdateSessionBegin(&session, FLEXFOX_FLASH_PAGE_BYTES, imageCrc32),
		"image_without_trailer_page_is_rejected");
	expect(!bootUpdateSessionBegin(&session, 513, imageCrc32), "unaligned_image_is_rejected");
	expect(
		!bootUpdateSessionBegin(
			&session,
			FLEXFOX_FLASH_BYTES - FLEXFOX_APP_START_BYTES + FLEXFOX_FLASH_PAGE_BYTES,
			imageCrc32),
		"oversized_image_is_rejected");
}

void esp_power_policy_distinguishes_live_handoff_from_cold_recovery()
{
	expect(bootUpdatePreserveLiveEsp(true, false), "software_handoff_preserves_live_esp");
	expect(!bootUpdatePreserveLiveEsp(true, true), "power_start_never_assumes_live_esp");
	expect(!bootUpdatePreserveLiveEsp(false, false), "ordinary_reset_uses_cold_esp_sequence");
	expect(!bootUpdatePreserveLiveEsp(false, true), "ordinary_power_start_uses_cold_esp_sequence");
}

void reset_page_and_order_are_enforced()
{
	auto session = begunSession();
	expect(!bootUpdateSessionAllowsErase(&session, pageAddress(1)), "page_one_cannot_precede_reset_erase");
	expect(bootUpdateSessionAllowsErase(&session, FLEXFOX_APP_START_BYTES), "reset_page_erase_is_first");
	bootUpdateSessionNoteErase(&session, FLEXFOX_APP_START_BYTES);
	expect(!bootUpdateSessionAllowsWrite(&session, FLEXFOX_APP_START_BYTES, resetCrc), "reset_write_is_blocked_until_last");
	expect(!bootUpdateSessionAllowsErase(&session, pageAddress(2)), "out_of_order_erase_is_rejected");
	expect(bootUpdateSessionAllowsErase(&session, pageAddress(1)), "page_one_erase_is_allowed");
	bootUpdateSessionNoteErase(&session, pageAddress(1));
	expect(!bootUpdateSessionAllowsWrite(&session, pageAddress(2), page2Crc), "out_of_order_write_is_rejected");
	expect(bootUpdateSessionAllowsWrite(&session, pageAddress(1), page1Crc), "erased_page_write_is_allowed");
	bootUpdateSessionNoteWrite(&session, pageAddress(1), page1Crc);
	expect(bootUpdateSessionAllowsWrite(&session, pageAddress(1), page1Crc), "identical_write_retry_is_allowed");
	expect(!bootUpdateSessionAllowsWrite(&session, pageAddress(1), page2Crc), "changed_write_retry_is_rejected");
}

void complete_transaction_is_the_only_session_run_path()
{
	auto session = begunSession();
	expect(!bootUpdateSessionMayRunApplication(&session, true, true), "marked_application_cannot_run_before_commit");
	bootUpdateSessionNoteErase(&session, FLEXFOX_APP_START_BYTES);
	const std::uint16_t crcs[] = {page1Crc, page2Crc, trailerCrc};
	for(std::uint16_t page = 1; page < session.page_count; page++)
	{
		const auto address = pageAddress(page);
		expect(bootUpdateSessionAllowsErase(&session, address), "next_page_erase_is_allowed");
		bootUpdateSessionNoteErase(&session, address);
		expect(bootUpdateSessionAllowsWrite(&session, address, crcs[page - 1]), "next_page_write_is_allowed");
		bootUpdateSessionNoteWrite(&session, address, crcs[page - 1]);
	}
	expect(bootUpdateSessionReadyForResetWrite(&session), "all_non_reset_pages_unlock_reset_write");
	expect(bootUpdateSessionAllowsWrite(&session, FLEXFOX_APP_START_BYTES, resetCrc), "reset_write_is_final");
	bootUpdateSessionNoteWrite(&session, FLEXFOX_APP_START_BYTES, resetCrc);
	expect(bootUpdateSessionMayRunApplication(&session, true, true), "committed_session_may_run");
	expect(bootUpdateSessionAllowsWrite(&session, FLEXFOX_APP_START_BYTES, resetCrc), "final_write_retry_is_idempotent");
	expect(!bootUpdateSessionAllowsErase(&session, pageAddress(1)), "committed_session_rejects_more_erases");
}

void every_interruption_requires_a_fresh_full_session()
{
	for(std::uint16_t interruptionPage = 0; interruptionPage < 4; interruptionPage++)
	{
		auto interrupted = begunSession();
		bootUpdateSessionNoteErase(&interrupted, FLEXFOX_APP_START_BYTES);
		for(std::uint16_t page = 1; page <= interruptionPage && page < interrupted.page_count; page++)
		{
			const auto address = pageAddress(page);
			bootUpdateSessionNoteErase(&interrupted, address);
			bootUpdateSessionNoteWrite(&interrupted, address, (std::uint16_t)(0x1000U + page));
		}

		BootUpdateSession afterReset;
		bootUpdateSessionReset(&afterReset);
		expect(
			!bootUpdateSessionMayRunApplication(&afterReset, false, false),
			"erased_vector_stays_in_bootloader_after_interruption");
		expect(bootUpdateSessionBegin(&afterReset, imageBytes, imageCrc32), "recovery_starts_new_session");
		expect(afterReset.next_page == 1U, "recovery_replays_from_page_one");
	}
}

void ordinary_non_update_exit_remains_available()
{
	BootUpdateSession session;
	bootUpdateSessionReset(&session);
	expect(bootUpdateSessionMayRunApplication(&session, false, true), "ordinary_serial_entry_can_return_to_app");
	expect(!bootUpdateSessionMayRunApplication(&session, true, true), "persistent_marker_blocks_ordinary_return");
}

} // namespace

int main()
{
	invalid_images_fail_closed();
	esp_power_policy_distinguishes_live_handoff_from_cold_recovery();
	reset_page_and_order_are_enforced();
	complete_transaction_is_the_only_session_run_path();
	every_interruption_requires_a_fresh_full_session();
	ordinary_non_update_exit_remains_available();
	std::cout << "All AVR boot update session tests passed\n";
	return EXIT_SUCCESS;
}
