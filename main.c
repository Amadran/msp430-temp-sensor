#include "lcd/lcd.h"

int main(void)
{
	// stop watch dog timer
	WDTCTL = WDTPW | WDTHOLD;

	// configure data port (P3) and control port (P2) pins as output
	dp_set_output();
	CTRLDIR |= (RSBIT | RWBIT | EBIT);
	CTRLOUT = 0; 

	// setup debug LED (P1.0)
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;

	busy_poll(NULL);
	function_set(1, 1, 0);
	busy_poll(NULL);
	display_ctrl(1, 1, 1);
	busy_poll(NULL);
	entry_mode_set(1, 0);
	busy_poll(NULL);
	set_ddram_addr(0);
	busy_poll(NULL);

    char str[] = "test hello";
    uint8_t strlen = 10;
    set_ddram_addr(0);

	while (1) {
		for (uint8_t i = 0; i < strlen; i++) {
            uint8_t bfaddr;
            busy_poll(&bfaddr);
            if ((bfaddr & 0b01111111) == 0x10) {
                set_ddram_addr(0x40);
            }
            else if ((bfaddr & 0b01111111) == 0x4f) {
                set_ddram_addr(0x00);
            }
            write_char(str[i]);
        }
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
        DELAY(NUMCYCLES_MAX);
	}

	return 0;
}

