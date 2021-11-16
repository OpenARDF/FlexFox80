#include <atmel_start.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "src/linkbus.h"

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	linkbus_send_text("ABC...\n");
	/* Replace with your application code */
	while (1) {
	}
}