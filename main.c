#include <msp430.h>
#include <stdint.h>

// pin defines
#define DPDIR				P3DIR
#define DPOUT				P3OUT
#define DPIN 				P3IN
#define DPREN				P3REN
#define CTRLDIR				P2DIR
#define CTRLOUT				P2OUT

// P2:	 7	  6	   5	4	 3	  2	   1	0
// 		 -- | -- | -- | -- | RS | RW | -- | E
#define RSBIT				BIT3
#define RWBIT				BIT2
#define EBIT				BIT0
#define RSBIT_POS			3
#define RWBIT_POS			2
#define EBIT_POS			0

// LCD "opcodes"
#define OPCODE_FSET			0b00100000
#define FSET_DL_BIT			4
#define FSET_N_BIT			3
#define FSET_F_BIT			2

#define OPCODE_DISPCTRL		0b00001000
#define DISPCTRL_D_BIT		2
#define DISPCTRL_C_BIT		1
#define DISPCTRL_B_BIT		0

#define OPCODE_SETDDRAMADDR	0b10000000

#define OPCODE_ENTRYMODE	0b00000100
#define ENTRYMODE_ID_BIT	1
#define ENTRYMODE_S_BIT		0

#define OPCODE_DISPCLR		0b00000001

// e, rw, rs must be 0 or 1
void set_ctrlout(uint8_t e, uint8_t rw, uint8_t rs) {
	CTRLOUT = ((unsigned char)e << EBIT_POS) |
			  ((unsigned char)rw << RWBIT_POS) |
			  ((unsigned char)rs << RSBIT_POS);
}

// dl:	0 = 4 bits		1 = 8 bits
// n:	0 = 1 line		1 = 2 lines
// f:	0 = 5*8 chars	1 = 5*10 chars
void function_set(uint8_t dl, uint8_t n, uint8_t f) {
	set_ctrlout(1, 0, 0);
	DPOUT = OPCODE_FSET | (dl << FSET_DL_BIT) | (n << FSET_N_BIT) | (f << FSET_F_BIT);
	__delay_cycles(0xffff);
	CTRLOUT &= ~EBIT;
}

// id:	0 = increment		1 = decrement
// s:	0 = no shift		1 = display shift
void entry_mode_set(uint8_t id, uint8_t s) {
	set_ctrlout(1, 0, 0);
	DPOUT = OPCODE_ENTRYMODE | (id << ENTRYMODE_ID_BIT) | (s << ENTRYMODE_S_BIT);
	__delay_cycles(0xffff);
	CTRLOUT &= ~EBIT;	
}

// d:	0 = display off		1 = display on
// c:	0 = cursor off		1 = cursor on
// b:	0 = blinking off	1 = blinking on
void display_ctrl(uint8_t d, uint8_t c, uint8_t b) {
	set_ctrlout(1, 0, 0);
	DPOUT = OPCODE_DISPCTRL | (d << DISPCTRL_D_BIT) | (c << DISPCTRL_C_BIT) | (b << DISPCTRL_B_BIT);
	__delay_cycles(0xffff);
	CTRLOUT &= ~EBIT;
}

void display_clear() {	
	set_ctrlout(1, 0, 0);
	DPOUT = OPCODE_DISPCLR;
	__delay_cycles(0xffff);
	CTRLOUT &= ~EBIT;
}

void set_ddram_addr(uint8_t addr) {
	set_ctrlout(1, 0, 0);
	DPOUT = OPCODE_SETDDRAMADDR | (0b01111111 & addr); // make sure opcode bit is not set in addr
	__delay_cycles(0xffff);
	CTRLOUT &= ~EBIT;
}

void write_char(char c) {
	set_ctrlout(1, 0, 1);
	DPOUT = c;
	__delay_cycles(0xffff);
	CTRLOUT &= ~EBIT;
	CTRLOUT &= ~RSBIT;
}

void dp_set_input() {
	DPDIR = 0;
	DPREN = 0xff;
	DPOUT = 0xff;
}

void dp_set_output() {	
	DPDIR = 0xff;
	DPREN = 0;
	DPOUT = 0;
}

void is_busy_poll() {
	set_ctrlout(1, 1, 0);
	dp_set_input();

	P1OUT &= ~BIT0;
	uint8_t bf;
	do {
		CTRLOUT |= EBIT;
		__delay_cycles(0xffff);
		bf = DPIN;
		CTRLOUT &= ~EBIT;
	} while (bf & 0b10000000);
	P1OUT |= BIT0;

	dp_set_output();
}

void init() {
	// delay for at least 40 ms (2.6124 * 16)
	for (volatile uint8_t i = 0; i < 16; i++) {
		__delay_cycles(0xffff);
	}

	function_set(1, 0, 0);

	// delay for at least 4.1 ms (2.6124 * 2)
	for (volatile uint8_t i = 0; i < 2; i++) {
		__delay_cycles(0xffff);
	}

	function_set(1, 0, 0);

	// delay for at least 100 us (2500 cycles)
	__delay_cycles(2500);

	function_set(1, 0, 0);

	is_busy_poll();

	// real function set
	function_set(1, 1, 0);

	is_busy_poll();

	display_ctrl(0, 0, 0);

	is_busy_poll();

	display_clear();

	is_busy_poll();

	entry_mode_set(1, 0);
}

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

//	init();

	//display_clear();

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

