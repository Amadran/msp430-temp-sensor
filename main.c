#include "lcd/lcd.h"

int main(void)
{
	// stop watch dog timer
	WDTCTL = WDTPW | WDTHOLD;

	for (volatile unsigned char i = 0; i < 10; i++) {
		__delay_cycles(0xffff);
	}

	// configure data port (P3) and control port (P2) pins as output
	dp_set_output();

	CTRLDIR |= (RSBIT | RWBIT | EBIT);
	CTRLOUT = 0; 

	// debug LED (P1.0)
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;

	// set N = 1 (2 lines)
	is_busy_poll();
	function_set(1, 1, 0);	
	is_busy_poll();
	display_ctrl(1, 1, 1);
	is_busy_poll();
	entry_mode_set(1, 0);
	is_busy_poll();
	set_ddram_addr(0);
	is_busy_poll();

//	volatile unsigned char flag = 0;
	char c = 0;
	while (1) {
		for (volatile unsigned short i = 0; i < 10; i++) {
			__delay_cycles(0xffff);
		}
		is_busy_poll();
		P1OUT ^= BIT0;

		write_char(c++);
		/*set_ddram_addr(0);
		if (!flag) {
			is_busy_poll();
			write_char('f');
			flag = 1;
		}
		else {
			is_busy_poll();
			write_char('F');
			flag = 0;
		}*/
	}
	return 0;
}

