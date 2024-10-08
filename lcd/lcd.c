#include "lcd.h"

void clear_display() {	
	set_ctrlout(1, 0, 0);
	DPOUT = OPCODE_DISPCLR;
	DELAY(NUMCYCLES_40US);
	CTRLOUT &= ~EBIT;
}

void return_home() {
    return; // TODO
}

// id:	0 = increment		1 = decrement
// s:	0 = no shift		1 = display shift
void entry_mode_set(uint8_t id, uint8_t s) {
	set_ctrlout(1, 0, 0);
	DPOUT = OPCODE_ENTRYMODE | (id << ENTRYMODE_ID_BIT) | (s << ENTRYMODE_S_BIT);
	DELAY(NUMCYCLES_40US);
	CTRLOUT &= ~EBIT;	
}

// d:	0 = display off		1 = display on
// c:	0 = cursor off		1 = cursor on
// b:	0 = blinking off	1 = blinking on
void display_ctrl(uint8_t d, uint8_t c, uint8_t b) {
	set_ctrlout(1, 0, 0);
	DPOUT = OPCODE_DISPCTRL | (d << DISPCTRL_D_BIT) | (c << DISPCTRL_C_BIT) | (b << DISPCTRL_B_BIT);
    DELAY(NUMCYCLES_40US);
	CTRLOUT &= ~EBIT;
}

void cursor_or_display_shift(uint8_t sc, uint8_t rl) {
    (void)sc;
    (void)rl;
    return; // TODO
}

// dl:	0 = 4 bits		1 = 8 bits
// n:	0 = 1 line		1 = 2 lines
// f:	0 = 5*8 chars	1 = 5*10 chars
void function_set(uint8_t dl, uint8_t n, uint8_t f) {
	set_ctrlout(1, 0, 0);
	DPOUT = OPCODE_FSET | (dl << FSET_DL_BIT) | (n << FSET_N_BIT) | (f << FSET_F_BIT);
	DELAY(NUMCYCLES_40US);
	CTRLOUT &= ~EBIT;
}

void set_cgram_addr(uint8_t addr) {
    (void)addr;
    return; // TODO
}

void set_ddram_addr(uint8_t addr) {
	set_ctrlout(1, 0, 0);
	DPOUT = OPCODE_SETDDRAMADDR | (0b01111111 & addr); // make sure opcode bit is not set in addr
	DELAY(NUMCYCLES_40US);
	CTRLOUT &= ~EBIT;
}

void read_busy_flag_and_addr(uint8_t* bfaddr) {
    if (bfaddr != NULL) {
        set_ctrlout(1, 1, 0);
        dp_set_input();	
        *bfaddr = DPIN;
        CTRLOUT &= ~EBIT;
        dp_set_output(); 
    }
}

void write_char(char c) {
	set_ctrlout(1, 0, 1);
	DPOUT = c;
	DELAY(NUMCYCLES_45US);
	CTRLOUT &= ~EBIT;
	CTRLOUT &= ~RSBIT;
}

// e, rw, rs must be 0 or 1
void set_ctrlout(uint8_t e, uint8_t rw, uint8_t rs) {
	CTRLOUT = ((unsigned char)e << EBIT_POS) |
			  ((unsigned char)rw << RWBIT_POS) |
			  ((unsigned char)rs << RSBIT_POS);
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

void busy_poll(uint8_t* bfaddr) {
    if (bfaddr != NULL) {
        P1OUT ^= BIT0;
        do {
            read_busy_flag_and_addr(bfaddr);
        } while (*bfaddr & 0b10000000);
    }
}

void init() {
	// delay for at least 40 ms (2.6124 * 16)
	for (volatile uint8_t i = 0; i < 16; i++) {
		DELAY(NUMCYCLES_40US);
	}

	function_set(1, 0, 0);

	// delay for at least 4.1 ms (2.6124 * 2)
	for (volatile uint8_t i = 0; i < 2; i++) {
		DELAY(NUMCYCLES_40US);
	}

	function_set(1, 0, 0);

	// delay for at least 100 us (2500 cycles)
	__delay_cycles(2500);

	function_set(1, 0, 0);

	busy_poll(NULL);

	// real function set
	function_set(1, 1, 0);

	busy_poll(NULL);

	display_ctrl(0, 0, 0);

	busy_poll(NULL);

	clear_display();

	busy_poll(NULL);

	entry_mode_set(1, 0);
}
