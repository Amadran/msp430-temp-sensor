#include <msp430.h>
#include <stdint.h>
#include <stddef.h>

// LCD pin defines
// DP == "data port"
// CTRL == "control"
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

// timing constants and functions
#define NUMCYCLES_40US          1000
#define NUMCYCLES_45US          1125
#define NUMCYCLES_1P6MS         40000
#define NUMCYCLES_MAX           0xffff  // == 2.62144 ms
#define DELAY(x)                __delay_cycles(x)



//// LCD functions
void display_clear();

void return_home();

// id:	0 = increment		1 = decrement
// s:	0 = no shift		1 = display shift
void entry_mode_set(uint8_t id, uint8_t s);

// d:	0 = display off		1 = display on
// c:	0 = cursor off		1 = cursor on
// b:	0 = blinking off	1 = blinking on
void display_ctrl(uint8_t d, uint8_t c, uint8_t b);

void cursor_or_display_shift(uint8_t sc, uint8_t rl);

// dl:	0 = 4 bits		1 = 8 bits
// n:	0 = 1 line		1 = 2 lines
// f:	0 = 5*8 chars	1 = 5*10 chars
void function_set(uint8_t dl, uint8_t n, uint8_t f);

void set_cgram_addr(uint8_t addr);

void set_ddram_addr(uint8_t addr);

void read_busy_flag_and_addr(uint8_t* bf_addr);

void write_char(char c);

// void read_char(char* c); // TODO: do I want this?



//// other functions

// e, rw, rs must be 0 or 1
void set_ctrlout(uint8_t e, uint8_t rw, uint8_t rs); 

void dp_set_input();

void dp_set_output();

void busy_poll();

void init();
