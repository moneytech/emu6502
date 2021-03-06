#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "test6502.h"
#include "opcodes.h"
#include "state.h"
#include "disassembler.h"
#include "cpu.h"
#include "test_framework.h"



////////////////////////////////////////


//// LDA

void test_LDA_IMM() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDA_IMM, 0xAA }; //LDA #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}

void test_LDA_IMM_zero() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDA_IMM, 0x00 };
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0x00);
	assert_flag_z(&state, 1);

	test_cleanup(&state);
}


void test_LDA_ZP() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDA_ZP, 0x03, 0x00, 0xAA }; //LDA $3
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_ZPX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x01;

	//arrange
	char program[] = { LDA_ZPX, 0x02, 0x00, 0xAA }; //LDA $02,X
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_ZPX_wraparound() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0xFF;

	//arrange
	char program[] = { LDA_ZPX, 0x04, 0x00, 0xAA }; //LDA $04,X
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	//should wrap $FF+$04 around to $03
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_ABS() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDA_ABS, 0x01, 0x04 }; //LDA $0401
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x401] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_ABSX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x02;

	//arrange
	char program[] = { LDA_ABSX, 0x01, 0x04 }; //LDA $0401,x
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_ABSY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x02;

	//arrange
	char program[] = { LDA_ABSY, 0x01, 0x04 }; //LDA $0401,y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_INDX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x05;

	//arrange
	char program[] = { LDA_INDX, 0x3E }; //LDA ($3E, x)
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0043] = 0xA9;
	state.memory[0x0044] = 0x04;
	state.memory[0x04A9] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_INDX_wraparound() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDA_INDX, 0xFF }; //LDA ($FF, x)
	memcpy(state.memory + 0x200, program, sizeof(program));
	//$FF+0 -> $FF
	//value at $FF is $00, continuing at $00 with 04 -> $0400
	state.memory[0x00FF] = 0x00;
	state.memory[0x0000] = 0x04;
	state.memory[0x0100] = 0x33; //fake unwanted value
	state.memory[0x0400] = 0xEE; //correct final value
	state.pc = 0x200;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xEE);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_INDY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x05;

	//arrange
	char program[] = { LDA_INDY, 0x3E }; //LDA ($3E),y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x3E] = 0xA0; //0x04A0
	state.memory[0x3F] = 0x04;
	state.memory[0x04A5] = 0xAA; //address 0x04A0 + 0x05 = 0x04A5

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_INDY_wraparound() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDA_INDY, 0xFF }; //LDA ($FF), y
	memcpy(state.memory + 0x200, program, sizeof(program));
	//$FF+0 -> $FF
	//value at $FF is $00, continuing at $00 with 04 -> $0400
	state.memory[0x00FF] = 0x00;
	state.memory[0x0000] = 0x04;
	state.memory[0x0100] = 0x33; //fake unwanted value
	state.memory[0x0400] = 0xEE; //correct final value
	state.pc = 0x200;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xEE);

	//cleanup
	test_cleanup(&state);
}

//// ORA

void test_ORA_IMM() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_IMM, 0xA0 }; //ORA #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	test_cleanup(&state);
}

void test_ORA_ZP() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_ZP, 0x03, 0x00, 0xA0 }; //ORA $3
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_ORA_ZPX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x01;
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_ZPX, 0x02, 0x00, 0xA0 }; //ORA $2,x
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_ORA_ABS() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_ABS, 0x01, 0x04 }; //ORA $0401
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x401] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_ORA_ABSX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x02;
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_ABSX, 0x01, 0x04 }; //ORA $0401,x
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_ORA_ABSY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x02;
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_ABSY, 0x01, 0x04 }; //ORA $0401,y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_ORA_INDX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x05;
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_INDX, 0x3E }; //ORA ($3E, x)
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0043] = 0xA9;
	state.memory[0x0044] = 0x04;
	state.memory[0x04A9] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_ORA_INDY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x05;
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_INDY, 0x3E }; //ORA ($3E),y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x3E] = 0xA0; //0x04A0
	state.memory[0x3F] = 0x04;
	state.memory[0x04A5] = 0xA0; //address 0x04A0 + 0x05 = 0x04A5

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_ORA_IMM_Z() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { ORA_IMM, 0x00 }; //LDA #$55, AND #$55
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);
	test_step(&state);

	//assert	
	assertA(&state, 0x00);
	assert_flag_z(&state, 1);

	//cleanup
	test_cleanup(&state);
}

//// EOR

void test_EOR_IMM() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x0F;

	//arrange
	char program[] = { EOR_IMM, 0x4F }; //EOR #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0x40);

	test_cleanup(&state);
}

void test_EOR_ZP() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x0F;

	//arrange
	char program[] = { EOR_ZP, 0x03, 0x00, 0x4F }; //EOR $3
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0x40);

	//cleanup
	test_cleanup(&state);
}

void test_EOR_ZPX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x01;
	state.a = 0x0F;

	//arrange
	char program[] = { EOR_ZPX, 0x02, 0x00, 0x4F }; //EOR $2,x
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0x40);

	//cleanup
	test_cleanup(&state);
}

void test_EOR_ABS() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x0F;

	//arrange
	char program[] = { EOR_ABS, 0x01, 0x04 }; //EOR $0401
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x401] = 0x4F;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0x40);

	//cleanup
	test_cleanup(&state);
}

void test_EOR_ABSX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x02;
	state.a = 0x0F;

	//arrange
	char program[] = { EOR_ABSX, 0x01, 0x04 }; //EOR $0401,x
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0x4F;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0x40);

	//cleanup
	test_cleanup(&state);
}

void test_EOR_ABSY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x02;
	state.a = 0x0F;

	//arrange
	char program[] = { EOR_ABSY, 0x01, 0x04 }; //EOR $0401,y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0x4F;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0x40);

	//cleanup
	test_cleanup(&state);
}

void test_EOR_INDX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x05;
	state.a = 0x0F;

	//arrange
	char program[] = { EOR_INDX, 0x3E }; //EOR ($3E, x)
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0043] = 0xA9;
	state.memory[0x0044] = 0x04;
	state.memory[0x04A9] = 0x4F;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0x40);

	//cleanup
	test_cleanup(&state);
}

void test_EOR_INDY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x05;
	state.a = 0x0F;

	//arrange
	char program[] = { EOR_INDY, 0x3E }; //EOR ($3E),y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x3E] = 0xA0; //0x04A0
	state.memory[0x3F] = 0x04;
	state.memory[0x04A5] = 0x4F; //address 0x04A0 + 0x05 = 0x04A5

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0x40);

	//cleanup
	test_cleanup(&state);
}


void test_EOR_IMM_Z() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x77;

	//arrange
	char program[] = { EOR_IMM, 0x77 }; //EOR #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0x0);
	assert_flag_z(&state, 1);

	test_cleanup(&state);
}

//// AND

void test_AND_IMM() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0xAA;

	//arrange
	char program[] = { AND_IMM, 0xA0 }; //AND #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	test_cleanup(&state);
}

void test_AND_ZP() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0xAA;

	//arrange
	char program[] = { AND_ZP, 0x03, 0x00, 0xA0 }; //AND $3
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

void test_AND_ZPX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x01;
	state.a = 0xAA;

	//arrange
	char program[] = { AND_ZPX, 0x02, 0x00, 0xA0 }; //AND $2,x
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

void test_AND_ABS() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0xAA;

	//arrange
	char program[] = { AND_ABS, 0x01, 0x04 }; //AND $0401
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x401] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

void test_AND_ABSX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x02;
	state.a = 0xAA;

	//arrange
	char program[] = { AND_ABSX, 0x01, 0x04 }; //AND $0401,x
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

void test_AND_ABSY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x02;
	state.a = 0xAA;

	//arrange
	char program[] = { AND_ABSY, 0x01, 0x04 }; //AND $0401,y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

void test_AND_INDX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x05;
	state.a = 0xAA;

	//arrange
	char program[] = { AND_INDX, 0x3E }; //AND ($3E, x)
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0043] = 0xA9;
	state.memory[0x0044] = 0x04;
	state.memory[0x04A9] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

void test_AND_INDY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x05;
	state.a = 0xAA;

	//arrange
	char program[] = { AND_INDY, 0x3E }; //AND ($3E),y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x3E] = 0xA0; //0x04A0
	state.memory[0x3F] = 0x04;
	state.memory[0x04A5] = 0xA0; //address 0x04A0 + 0x05 = 0x04A5

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

void test_AND_IMM_Z() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDA_IMM, 0x55, AND_IMM, 0xAA }; //LDA #$55, AND #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);
	test_step(&state);

	//assert	
	assertA(&state, 0x00);
	assert_flag_z(&state, 1);

	//cleanup
	test_cleanup(&state);
}

//// LDX

void test_LDX_IMM() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDX_IMM, 0xAA }; //LDX #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertX(&state, 0xAA);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}

void test_LDX_IMM_zero() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDX_IMM, 0x00 }; //LDX #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertX(&state, 0x00);
	assert_flag_z(&state, 1);

	test_cleanup(&state);
}

void test_LDX_ZP() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDX_ZP, 0x03, 0x00, 0xAA }; //LDX $3
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertX(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDX_ZPY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x01;

	//arrange
	char program[] = { LDX_ZPY, 0x02, 0x00, 0xAA }; //LDX $02,Y
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertX(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDX_ABS() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDX_ABS, 0x01, 0x04 }; //LDX $0401
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x401] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertX(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDX_ABSY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x02;

	//arrange
	char program[] = { LDX_ABSY, 0x01, 0x04 }; //LDX $0401,y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertX(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

//// LDY

void test_LDY_IMM() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDY_IMM, 0xAA }; //LDY #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertY(&state, 0xAA);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}

void test_LDY_IMM_zero() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDY_IMM, 0x00 };
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertY(&state, 0x00);
	assert_flag_z(&state, 1);

	test_cleanup(&state);
}

void test_LDY_ZP() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDY_ZP, 0x03, 0x00, 0xAA }; //LDY $3
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertY(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDY_ZPX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x01;

	//arrange
	char program[] = { LDY_ZPX, 0x02, 0x00, 0xAA }; //LDY $02,X
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertY(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDY_ABS() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDY_ABS, 0x01, 0x04 }; //LDY $0401
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x401] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertY(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDY_ABSX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x02;

	//arrange
	char program[] = { LDY_ABSX, 0x01, 0x04 }; //LDY $0401,x
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertY(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

//// STX

void test_STX_ZP() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x99;

	//arrange
	char program[] = { STX_ZP, 0xFF }; //STX $FF
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0xFF, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STX_ZPY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0xA0;
	state.x = 0x99;

	//arrange
	char program[] = { STX_ZPY, 0x02 }; //STX $02,Y
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0xA2, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STX_ABS() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x99;

	//arrange
	char program[] = { STX_ABS, 0x01, 0x04 }; //STX $0401
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0x401, 0x99);

	//cleanup
	test_cleanup(&state);
}

//// STY

void test_STY_ZP() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x99;

	//arrange
	char program[] = { STY_ZP, 0xFF }; //STY $FF
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0xFF, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STY_ZPX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0xA0;
	state.y = 0x99;

	//arrange
	char program[] = { STY_ZPX, 0x02 }; //STY $02,X
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0xA2, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STY_ABS() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x99;

	//arrange
	char program[] = { STY_ABS, 0x01, 0x04 }; //STY $0401
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0x401, 0x99);

	//cleanup
	test_cleanup(&state);
}

// INX, INY, DEX, DEY

void test_DEX_wraparound() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { DEX };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertX(&state, 0xFF);
	assert_flag_n(&state, 1);
	assert_flag_z(&state, 0);

	test_cleanup(&state);
}

void test_DEX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0xE1;

	//arrange
	char program[] = { DEX };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertX(&state, 0xE0);
	assert_flag_n(&state, 1);
	assert_flag_z(&state, 0);

	test_cleanup(&state);
}

void test_DEY_wraparound() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { DEY };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertY(&state, 0xFF);
	assert_flag_n(&state, 1);
	assert_flag_z(&state, 0);

	test_cleanup(&state);
}

void test_DEY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0xE1;

	//arrange
	char program[] = { DEY };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertY(&state, 0xE0);
	assert_flag_n(&state, 1);
	assert_flag_z(&state, 0);

	test_cleanup(&state);
}

void test_INX_wraparound() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0xFF;

	//arrange
	char program[] = { INX };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertX(&state, 0x00);
	assert_flag_z(&state, 1);
	assert_flag_n(&state, 0);

	test_cleanup(&state);
}

void test_INX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0xE1;

	//arrange
	char program[] = { INX };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertX(&state, 0xE2);

	test_cleanup(&state);
}

void test_INY_wraparound() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0xFF;

	//arrange
	char program[] = { INY };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertY(&state, 0x00);
	assert_flag_z(&state, 1);

	test_cleanup(&state);
}

void test_INY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0xE1;

	//arrange
	char program[] = { INY };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertY(&state, 0xE2);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}

// TXA, TAX, TYA, TAY

void test_TXA() {
	State6502 state = create_blank_state();
	state.x = 0xEE;

	char program[] = { TXA };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertA(&state, 0xEE);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}

void test_TAX() {
	State6502 state = create_blank_state();
	state.a = 0xEE;

	char program[] = { TAX };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertX(&state, 0xEE);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}


void test_TYA() {
	State6502 state = create_blank_state();
	state.y = 0xEE;

	char program[] = { TYA };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertA(&state, 0xEE);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}


void test_TAY() {
	State6502 state = create_blank_state();
	state.a = 0xEE;

	char program[] = { TAY };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertY(&state, 0xEE);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}

//// INC, DEC

void test_INC_ZP() {
	State6502 state = create_blank_state();

	char program[] = { INC_ZP, 0xFF };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assert_memory(&state, 0xFF, 0x01);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 0);

	test_cleanup(&state);
}

void test_INC_ZP_wraparound() {
	State6502 state = create_blank_state();

	char program[] = { INC_ZP, 0xFF };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0xFF] = 0xFF;
	test_step(&state);

	assert_memory(&state, 0xFF, 0x00);
	assert_flag_z(&state, 1);
	assert_flag_n(&state, 0);

	test_cleanup(&state);
}

void test_INC_ZP_(byte operand, byte expected, byte expected_zero, byte expected_n) {
	State6502 state = create_blank_state();

	char program[] = { INC_ZP, 0xFF };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0xFF] = 0xFF;
	test_step(&state);

	assert_memory(&state, 0xFF, 0x00);
	assert_flag_z(&state, 1);
	assert_flag_n(&state, 0);

	test_cleanup(&state);
}

#define _ ,
#define __ ,

void test_INC_ZP_multiple() {
	//           A    =>   A    Z   N
	test_INC_ZP_(0x00 __ 0x01 _ 0 _ 0);
	test_INC_ZP_(0x7F __ 0x80 _ 0 _ 1);
	test_INC_ZP_(0xFF __ 0x00 _ 1 _ 0);
	test_INC_ZP_(0xFE __ 0xFF _ 0 _ 1);
}

void test_INC_ZPX() {
	State6502 state = create_blank_state();
	state.x = 0x02;
	char program[] = { INC_ZPX, 0xFD };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assert_memory(&state, 0xFF, 0x01);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 0);

	test_cleanup(&state);
}

void test_INC_ABS() {
	State6502 state = create_blank_state();
	char program[] = { INC_ABS, 0xFF, 0x01 };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assert_memory(&state, 0x1FF, 0x01);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 0);

	test_cleanup(&state);
}

void test_INC_ABSX() {
	State6502 state = create_blank_state();
	state.x = 0x02;
	char program[] = { INC_ABSX, 0xFD, 0x01 };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assert_memory(&state, 0x1FF, 0x01);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 0);

	test_cleanup(&state);
}

void test_DEC_ZP() {
	State6502 state = create_blank_state();

	char program[] = { DEC_ZP, 0xFF };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0xFF] = 0xFF;
	test_step(&state);

	assert_memory(&state, 0xFF, 0xFE);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}

void test_DEC_ZP_wraparound() {
	State6502 state = create_blank_state();

	char program[] = { DEC_ZP, 0xFF };
	memcpy(state.memory, program, sizeof(program));
	test_step(&state);

	assert_memory(&state, 0xFF, 0xFF);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}

// FLAGS

void test_SEC() {
	State6502 state = create_blank_state();

	char program[] = { SEC };
	memcpy(state.memory, program, sizeof(program));
	test_step(&state);

	assert_flag_c(&state, 1);

	test_cleanup(&state);
}

void test_CLC() {
	State6502 state = create_blank_state();
	state.flags.c = 1;
	char program[] = { CLC };
	memcpy(state.memory, program, sizeof(program));
	test_step(&state);

	assert_flag_c(&state, 0);

	test_cleanup(&state);
}

void test_SED() {
	State6502 state = create_blank_state();

	char program[] = { SED };
	memcpy(state.memory, program, sizeof(program));
	test_step(&state);

	assert_flag_d(&state, 1);

	test_cleanup(&state);
}

void test_CLD() {
	State6502 state = create_blank_state();
	state.flags.d = 1;
	char program[] = { CLD };
	memcpy(state.memory, program, sizeof(program));
	test_step(&state);

	assert_flag_d(&state, 0);

	test_cleanup(&state);
}

void test_SEI() {
	State6502 state = create_blank_state();

	char program[] = { SEI };
	memcpy(state.memory, program, sizeof(program));
	test_step(&state);

	assert_flag_i(&state, 1);

	test_cleanup(&state);
}

void test_CLI() {
	State6502 state = create_blank_state();
	state.flags.i = 1;
	char program[] = { CLI };
	memcpy(state.memory, program, sizeof(program));
	test_step(&state);

	assert_flag_i(&state, 0);

	test_cleanup(&state);
}

void test_CLV() {
	State6502 state = create_blank_state();
	state.flags.v = 1;
	char program[] = { CLV };
	memcpy(state.memory, program, sizeof(program));
	test_step(&state);

	assert_flag_v(&state, 0);

	test_cleanup(&state);
}

//// STA

void test_STA_ZP() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x99;

	//arrange
	char program[] = { STA_ZP, 0xFF }; //STA $FF
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0xFF, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STA_ZPX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0xA0;
	state.a = 0x99;

	//arrange
	char program[] = { STA_ZPX, 0x02 }; //STA $02,X
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0xA2, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STA_ABS() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x99;

	//arrange
	char program[] = { STA_ABS, 0x01, 0x04 }; //STA $0401
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0x401, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STA_ABSX() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x99;
	state.x = 0x05;

	//arrange
	char program[] = { STA_ABSX, 0x01, 0x04 }; //STA $0401,x
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0x406, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STA_ABSY() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x99;
	state.y = 0x05;

	//arrange
	char program[] = { STA_ABSY, 0x01, 0x04 }; //STA $0401,y
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0x406, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STA_INDX() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x99;
	state.x = 0x05;

	//arrange
	char program[] = { STA_INDX, 0x3E }; //STA ($3E,x)
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x43] = 0x01;
	state.memory[0x44] = 0x04;

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0x401, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STA_INDY() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x99;
	state.y = 0x05;

	//arrange
	char program[] = { STA_INDY, 0x3E }; //STA ($3E),y
	state.memory[0x3E] = 0xA0;
	state.memory[0x3F] = 0x04;
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0x4A5, 0x99);

	//cleanup
	test_cleanup(&state);
}

// PH, PL

void test_PHA() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x99;
	state.sp = 0xFF;

	//arrange
	char program[] = { PHA };
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0x1FF, 0x99);
	assert_sp(&state, 0xFE);

	//cleanup
	test_cleanup(&state);
}

void test_PLA() {
	//initialize
	State6502 state = create_blank_state();
	state.sp = 0xFE;
	state.memory[0x1FF] = 0x40;

	//arrange
	char program[] = { PLA };
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0x40);
	assert_sp(&state, 0xFF);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 0);


	//cleanup
	test_cleanup(&state);
}

void test_PLA_Z() {
	//initialize
	State6502 state = create_blank_state();
	state.sp = 0xFE;
	state.memory[0x1FF] = 0x00;

	//arrange
	char program[] = { PLA };
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0x00);
	assert_sp(&state, 0xFF);
	assert_flag_z(&state, 1);
	assert_flag_n(&state, 0);

	//cleanup
	test_cleanup(&state);
}

void test_PLA_N() {
	//initialize
	State6502 state = create_blank_state();
	state.sp = 0xFE;
	state.memory[0x1FF] = 0xBB;

	//arrange
	char program[] = { PLA };
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xBB);
	assert_sp(&state, 0xFF);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 1);

	//cleanup
	test_cleanup(&state);
}

void test_PHA_PLA() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDX_IMM, 0xFF, TXS, LDA_IMM, 0xAA, PHA, LDY_IMM, 0xBB, TYA, PHA, TXA, PHA };
	memcpy(state.memory, program, sizeof(program));

	//act
	for (int i = 0; i < 9; i++) {
		disassemble_6502(state.memory, state.pc);
		printf("\n");
		emulate_6502_op(&state);
	}

	//assert	
	assert_memory(&state, 0x1FF, 0xAA);
	assert_memory(&state, 0x1FE, 0xBB);
	assert_memory(&state, 0x1FD, 0xFF);
	assert_sp(&state, 0xFC);

	//cleanup
	test_cleanup(&state);
}

//// TXS, TSX, TYS, TSY

void test_TXS() {
	State6502 state = create_blank_state();
	state.x = 0xAA;
	char program[] = { TXS };
	memcpy(state.memory, program, sizeof(program));
	test_step(&state);

	assert_sp(&state, 0xAA);

	test_cleanup(&state);
}

void test_TXS_Z() {
	State6502 state = create_blank_state();
	//initial state of Z and N = 1
	state.flags.z = 1;
	state.flags.n = 1;
	state.x = 0x00;
	char program[] = { TXS };
	memcpy(state.memory, program, sizeof(program));
	test_step(&state);

	assert_sp(&state, 0x00);
	assert_flag_n(&state, 1);
	assert_flag_z(&state, 1);

	test_cleanup(&state);
}

void test_TSX() {
	State6502 state = create_blank_state();
	state.sp = 0xBB;
	char program[] = { TSX };
	memcpy(state.memory, program, sizeof(program));
	test_step(&state);

	assertX(&state, 0xBB);

	test_cleanup(&state);
}

// PHP, PLP

void test_PHP() {
	//initialize
	State6502 state = create_blank_state();
	state.flags.c = 1;
	state.flags.b = 1;
	state.flags.z = 1;
	state.flags.n = 1;
	state.flags.d = 1;
	state.flags.i = 1;
	state.flags.v = 1;

	//arrange
	char program[] = { PHP };
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_sp(&state, 0xFE);
	assert_memory(&state, 0x1FF, 0xFF);

	//cleanup
	test_cleanup(&state);
}

void test_PHP_no_flags() {
	//initialize
	State6502 state = create_blank_state();
	//no flags are set

	//arrange
	char program[] = { PHP };
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_sp(&state, 0xFE);
	byte expected_value = (1 << 4) | (1 << 5);
	assert_memory(&state, 0x1FF, expected_value);

	//cleanup
	test_cleanup(&state);
}

void test_PLP() {
	State6502 state = create_blank_state();
	state.sp = 0xFE;

	//arrange
	char program[] = { PLP };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x1FF] = 0xFF; //all flags should be on

	//act
	test_step(&state);

	//assert	
	assert_sp(&state, 0xFF);
	assert_flag_c(&state, 1);
	assert_flag_d(&state, 1);
	assert_flag_b(&state, 0);
	assert_flag_z(&state, 1);
	assert_flag_v(&state, 1);
	assert_flag_n(&state, 1);
	assert_flag_i(&state, 1);

	//cleanup
	test_cleanup(&state);
}

void test_PLP2() {
	State6502 state = create_blank_state();
	state.sp = 0xfe;

	//arrange
	char program[] = { PLP };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x1FF] = 0x04; //only flag i should be set

	//act
	test_step(&state);

	//assert	
	assert_sp(&state, 0xFF);
	assert_flag_c(&state, 0);
	assert_flag_d(&state, 0);
	assert_flag_b(&state, 0);
	assert_flag_z(&state, 0);
	assert_flag_v(&state, 0);
	assert_flag_n(&state, 0);
	assert_flag_i(&state, 1);

	//cleanup
	test_cleanup(&state);
}

void test_PHA_PLP() {
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDA_IMM, 0x04, PHA, PLP };
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	assertA(&state, 0x04);

	//assert	
	assert_sp(&state, 0xFF);
	assert_flag_c(&state, 0);
	assert_flag_d(&state, 0);
	assert_flag_b(&state, 0);
	assert_flag_z(&state, 0);
	assert_flag_v(&state, 0);
	assert_flag_n(&state, 0);
	assert_flag_i(&state, 1);

	//cleanup
	test_cleanup(&state);
}

//// JMP

void test_JMP() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { JMP_ABS, 0x04, 0x03 };
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_pc(&state, 0x0304);

	//cleanup
	test_cleanup(&state);
}

void test_JMP_IND() {
	//initialize 
	State6502 state = create_blank_state();

	//arrange
	char program[] = { JMP_IND, 0x04, 0x02 };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0204] = 0xAA;
	state.memory[0x0205] = 0x01;

	//act
	test_step(&state);

	//assert	
	assert_pc(&state, 0x01AA);

	//cleanup
	test_cleanup(&state);
}

void test_JMP_IND_wrap() {
	//initialize 
	State6502 state = create_blank_state();

	//arrange
	char program[] = { JMP_IND, 0xFF, 0x02 };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x02FF] = 0xAA;
	state.memory[0x0200] = 0x01;
	state.memory[0x0300] = 0xFF; //NOT this one!

	//act
	test_step(&state);

	//assert	
	assert_pc(&state, 0x1AA);

	//cleanup
	test_cleanup(&state);
}

// CMP, CPX, CPY

void test_CMP_ABS_equal() {
	State6502 state = create_blank_state();
	state.a = 0x1A;

	char program[] = { CMP_ABS, 0x45, 0x03 };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0345] = 0x1A;
	test_step(&state);

	assert_flag_z(&state, 0x01);
	assert_flag_n(&state, 0x00);
	assert_flag_c(&state, 0x01);

	test_cleanup(&state);
}

void test_CMP_ABS_greater() {
	State6502 state = create_blank_state();
	state.a = 0x30;

	char program[] = { CMP_ABS, 0x45, 0x03 };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0345] = 0x1A;
	test_step(&state);

	assert_flag_z(&state, 0x00);
	assert_flag_n(&state, 0x00);
	assert_flag_c(&state, 0x01); //as A > memory

	test_cleanup(&state);
}

void test_CMP_ABS_greater_2() {
	State6502 state = create_blank_state();
	state.a = 0x82;
	state.flags.n = 1;

	char program[] = { CMP_ABS, 0x45, 0x03 };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0345] = 0x1A;
	test_step(&state);

	assert_flag_z(&state, 0x00); // 0x82 != 0x1A
	assert_flag_n(&state, 0x00);
	assert_flag_c(&state, 0x01); // 0x82 > 0x1A

	test_cleanup(&state);
}

void test_CMP_ABS_less_than() {
	State6502 state = create_blank_state();
	state.a = 0x08;

	char program[] = { CMP_ABS, 0x45, 0x03 };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0345] = 0x1A;
	test_step(&state);

	assert_flag_z(&state, 0x00);
	assert_flag_n(&state, 0x01);
	assert_flag_c(&state, 0x00);

	test_cleanup(&state);
}

// CPX, CPY
void test_CPX_ABS() {
	State6502 state = create_blank_state();
	state.x = 0x82;
	state.flags.n = 1;

	char program[] = { CPX_ABS, 0x45, 0x03 };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0345] = 0x1A;
	test_step(&state);

	assert_flag_z(&state, 0x00); // 0x82 != 0x1A
	assert_flag_n(&state, 0x00);
	assert_flag_c(&state, 0x01); // 0x82 > 0x1A

	test_cleanup(&state);
}

void test_CPY_ABS() {
	State6502 state = create_blank_state();
	state.y = 0x82;
	state.flags.n = 1;

	char program[] = { CPY_ABS, 0x45, 0x03 };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0345] = 0x1A;
	test_step(&state);

	assert_flag_z(&state, 0x00); // 0x82 != 0x1A
	assert_flag_n(&state, 0x00);
	assert_flag_c(&state, 0x01); // 0x82 > 0x1A

	test_cleanup(&state);
}

//// SBC

void test_SBC_IMM() {
	State6502 state = create_blank_state();
	state.a = 0x08;

	char program[] = { SBC_IMM, 0x06 };
	memcpy(state.memory, program, sizeof(program));
	test_step(&state);

	assertA(&state, 0x02);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 0);
	assert_flag_c(&state, 1);
	assert_flag_v(&state, 0);

	test_cleanup(&state);
}

void test_SBC_IMM_carry() {
	State6502 state = create_blank_state();
	state.a = 0x40;
	state.flags.c = 1;

	char program[] = { SBC_IMM, 0x60 };
	memcpy(state.memory, program, sizeof(program));
	test_step(&state);

	//0x100 + 0x40 - 0x60 = 0xE0
	assertA(&state, 0xE0);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 0);
	assert_flag_c(&state, 1);
	assert_flag_v(&state, 0);

	test_cleanup(&state);
}

void test_SBC_IMM_(byte a, byte c, byte operand, byte expected_a, byte expected_n, byte expected_z, byte expected_c, byte expected_v) {
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	State6502 state = create_blank_state();
	state.a = a;
	state.flags.c = c;
	char program[] = { SBC_IMM, operand };
	memcpy(state.memory, program, sizeof(program));
	//act
	test_step(&state);
	//assert
	assertA(&state, expected_a);
	assert_flag_n(&state, expected_n);
	assert_flag_z(&state, expected_z);
	assert_flag_c(&state, expected_c);
	assert_flag_v(&state, expected_v);
}


void test_SBC_IMM_multiple() {
	//A, C, OP => A, N, Z, C, V
	test_SBC_IMM_(0x08, 0, /* OP */ 0x01, /*A*/ 0x06, /*N*/ 0, /*Z*/ 0, /*C*/ 1, /*V*/ 0); //regular subtraction without carry bit
	test_SBC_IMM_(0x08, 1, /* OP */ 0x01, /*A*/ 0x07, /*N*/ 0, /*Z*/ 0, /*C*/ 1, /*V*/ 0); //regular subtraction with carry bit
	test_SBC_IMM_(0x0A, 1, /* OP */ 0x0A, /*A*/ 0x00, /*N*/ 0, /*Z*/ 1, /*C*/ 1, /*V*/ 0); //zero 
	test_SBC_IMM_(0x0A, 0, /* OP */ 0x0A, /*A*/ 0xFF, /*N*/ 1, /*Z*/ 0, /*C*/ 0, /*V*/ 0); //negative
	test_SBC_IMM_(0x04, 1, /* OP */ 0x06, /*A*/ 0xFE, /*N*/ 1, /*Z*/ 0, /*C*/ 0, /*V*/ 0); //borrow from carry bit - 4 + 256 - 6 = 255
	test_SBC_IMM_(0x04, 0, /* OP */ 0x06, /*A*/ 0xFD, /*N*/ 1, /*Z*/ 0, /*C*/ 0, /*V*/ 0); //borrow from carry bit - 4 + 256 - 6 = 254
	//source: http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
	//C7 - carry, B - Borrow, S7 - N, V - V
	test_SBC_IMM_(0x50, 1, /* OP */ 0xF0, /*A*/ 0x60, /*N*/ 0, /*Z*/ 0, /*C*/ 0, /*V*/ 0); //Unsigned borrow but no signed overflow
	test_SBC_IMM_(0x50, 1, /* OP */ 0xB0, /*A*/ 0xA0, /*N*/ 1, /*Z*/ 0, /*C*/ 0, /*V*/ 1); //Unsigned borrow and signed overflow
	test_SBC_IMM_(0x50, 1, /* OP */ 0x70, /*A*/ 0xE0, /*N*/ 1, /*Z*/ 0, /*C*/ 0, /*V*/ 0); //Unsigned borrow but no signed overflow
	test_SBC_IMM_(0x50, 1, /* OP */ 0x30, /*A*/ 0x20, /*N*/ 0, /*Z*/ 0, /*C*/ 1, /*V*/ 0); //No unsigned borrow or signed overflow
	test_SBC_IMM_(0xD0, 1, /* OP */ 0xF0, /*A*/ 0xE0, /*N*/ 1, /*Z*/ 0, /*C*/ 0, /*V*/ 0); //Unsigned borrow but no signed overflow
	test_SBC_IMM_(0xD0, 1, /* OP */ 0xB0, /*A*/ 0x20, /*N*/ 0, /*Z*/ 0, /*C*/ 1, /*V*/ 0); //No unsigned borrow or signed overflow
	test_SBC_IMM_(0xD0, 1, /* OP */ 0x70, /*A*/ 0x60, /*N*/ 0, /*Z*/ 0, /*C*/ 1, /*V*/ 1); //No unsigned borrow but signed overflow	
	test_SBC_IMM_(0xD0, 1, /* OP */ 0x30, /*A*/ 0xA0, /*N*/ 1, /*Z*/ 0, /*C*/ 1, /*V*/ 0); //No unsigned borrow or signed overflow	
}

// ADC

void test_ADC_IMM_exec(byte a, byte c, byte operand, byte expected_a, byte expected_n, byte expected_z, byte expected_c, byte expected_v) {
	State6502 state = create_blank_state();
	state.a = a;
	state.flags.c = c;
	char program[] = { ADC_IMM, operand };
	memcpy(state.memory, program, sizeof(program));
	//act
	test_step(&state);
	//assert
	assertA(&state, expected_a);
	assert_flag_n(&state, expected_n);
	assert_flag_z(&state, expected_z);
	assert_flag_c(&state, expected_c);
	assert_flag_v(&state, expected_v);
}

void test_ADC_IMM_multiple() {
	//A, C, OP => A, N, Z, C, V
	test_ADC_IMM_exec(2, 0, 3, 5, 0, 0, 0, 0); //straight addition
	test_ADC_IMM_exec(2, 1, 3, 6, 0, 0, 0, 0); //straight addition with carry
	test_ADC_IMM_exec(2, 0, 254, 0, 0, 1, 1, 0); //carry and zero
	test_ADC_IMM_exec(2, 0, 253, 255, 1, 0, 0, 0); //just negative
	test_ADC_IMM_exec(253, 0, 6, 3, 0, 0, 1, 0); //carry
	test_ADC_IMM_exec(254, 1, 1, 0, 0, 1, 1, 0); //carry and zero
	test_ADC_IMM_exec(125, 1, 2, 128, 1, 0, 0, 1); //negative and overflow
}

// BIT
void test_BIT_exec(byte a, byte expected_a, byte expected_n, byte expected_v, byte expected_z) {
	State6502 state = create_blank_state();
	state.a = a;
	char program[] = { BIT_ABS, 0x45, 0x03 };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0345] = 0xF3;
	//act
	test_step(&state);
	//assert
	assertA(&state, expected_a);
	assert_flag_n(&state, expected_n);
	assert_flag_z(&state, expected_z);
	assert_flag_v(&state, expected_v);
}

void test_BIT_multiple() {
	// A => A N V Z
	test_BIT_exec(128, 128, 1, 1, 0);
	test_BIT_exec(5, 5, 1, 1, 0);
	test_BIT_exec(4, 4, 1, 1, 1); // 128 & 4 = 0 -> Z = 1
	test_BIT_exec(3, 3, 1, 1, 0);
}

// JSR and RTS
void test_JSR() {
	State6502 state = create_blank_state();
	char program[] = { NOP, JSR_ABS, 0x23, 0x01 };
	memcpy(state.memory, program, sizeof(program));
	//act
	test_step(&state);
	test_step(&state);
	//assert
	assert_pc(&state, 0x0123);
	assert_memory(&state, 0x1FF, 0x00);
	assert_memory(&state, 0x1FE, 0x03);
	assert_sp(&state, 0xFD);
}

void test_RTS() {
	State6502 state = create_blank_state();
	state.memory[STACK_HOME + 0xFF] = 0x01;
	state.memory[STACK_HOME + 0xFE] = 0x23;
	state.sp = 0xFD;
	char program[] = { RTS };
	memcpy(state.memory, program, sizeof(program));
	//act
	test_step(&state);
	//assert
	assert_pc(&state, 0x0124);
	assert_sp(&state, 0xFF);
}

void test_JSR_RTS() {
	State6502 state = create_blank_state();
	char program[] = { JSR_ABS, 0x06, 0x00, LDA_IMM, 0xAA, BRK, LDX_IMM, 0xBB, RTS };
	memcpy(state.memory, program, sizeof(program));
	//act	
	test_step_until_break(&state);
	//assert
	assert_pc(&state, 0x0006);
	assert_sp(&state, 0xFF);
	assertA(&state, 0xAA);
	assertX(&state, 0xBB);
}

void test_BRK() {
	State6502 state = create_blank_state();
	char program[] = { BRK };
	memcpy(state.memory, program, sizeof(program));
	//act	
	test_step(&state);
	//assert
	assert_pc(&state, 0x0001);
	assert_flag_b(&state, 1);
}

//BEQ, BCC, ...

void test_branch(byte opcode, byte n, byte v, byte z, byte c, word expected_pc) {
	State6502 state = create_blank_state();
	state.flags.n = n;
	state.flags.v = v;
	state.flags.z = z;
	state.flags.c = c;
	char program[] = { opcode, 0xFE };
	memcpy(state.memory, program, sizeof(program));
	//act
	test_step(&state);
	//assert
	assert_pc(&state, expected_pc);
}

void test_branching_multiple() {
	test_branch(BEQ_REL, /*N*/ 0, /*V*/ 0, /*Z*/ 0, /*C*/ 0, /*EXP_PC*/0x02);
	test_branch(BEQ_REL, /*N*/ 0, /*V*/ 0, /*Z*/ 1, /*C*/ 0, /*EXP_PC*/0x00);
	test_branch(BNE_REL, /*N*/ 0, /*V*/ 0, /*Z*/ 0, /*C*/ 0, /*EXP_PC*/0x00);
	test_branch(BNE_REL, /*N*/ 0, /*V*/ 0, /*Z*/ 1, /*C*/ 0, /*EXP_PC*/0x02);
	test_branch(BCC_REL, /*N*/ 0, /*V*/ 0, /*Z*/ 0, /*C*/ 0, /*EXP_PC*/0x00);
	test_branch(BCC_REL, /*N*/ 0, /*V*/ 0, /*Z*/ 0, /*C*/ 1, /*EXP_PC*/0x02);
	test_branch(BCS_REL, /*N*/ 0, /*V*/ 0, /*Z*/ 0, /*C*/ 1, /*EXP_PC*/0x00);
	test_branch(BCS_REL, /*N*/ 0, /*V*/ 0, /*Z*/ 0, /*C*/ 0, /*EXP_PC*/0x02);
	test_branch(BMI_REL, /*N*/ 1, /*V*/ 0, /*Z*/ 0, /*C*/ 0, /*EXP_PC*/0x00);
	test_branch(BMI_REL, /*N*/ 0, /*V*/ 0, /*Z*/ 0, /*C*/ 0, /*EXP_PC*/0x02);
	test_branch(BPL_REL, /*N*/ 0, /*V*/ 0, /*Z*/ 0, /*C*/ 0, /*EXP_PC*/0x00);
	test_branch(BPL_REL, /*N*/ 1, /*V*/ 0, /*Z*/ 0, /*C*/ 0, /*EXP_PC*/0x02);
	test_branch(BVC_REL, /*N*/ 0, /*V*/ 0, /*Z*/ 0, /*C*/ 0, /*EXP_PC*/0x00);
	test_branch(BVC_REL, /*N*/ 0, /*V*/ 1, /*Z*/ 0, /*C*/ 0, /*EXP_PC*/0x02);
	test_branch(BVS_REL, /*N*/ 0, /*V*/ 1, /*Z*/ 0, /*C*/ 0, /*EXP_PC*/0x00);
	test_branch(BVS_REL, /*N*/ 0, /*V*/ 0, /*Z*/ 0, /*C*/ 0, /*EXP_PC*/0x02);
}

// RTI

void test_RTI() {
	State6502 state = create_blank_state();
	state.sp = 0xfc;

	//arrange
	char program[] = { RTI };
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x1FF] = 0x12; //return address 
	state.memory[0x1FE] = 0x34; //return address 
	state.memory[0x1FD] = 0x80; //only flag n should be set

	//act
	test_step(&state);

	//assert	
	assert_sp(&state, 0xFF);
	assert_pc(&state, 0x1234);
	assert_flag_c(&state, 0);
	assert_flag_d(&state, 0);
	assert_flag_b(&state, 0);
	assert_flag_z(&state, 0);
	assert_flag_v(&state, 0);
	assert_flag_n(&state, 1);
	assert_flag_i(&state, 0);

	//cleanup
	test_cleanup(&state);
}

// ASL

void test_ASL_ACC(byte a, byte expected_a, byte expected_c) {
	State6502 state = create_blank_state();
	state.a = a;

	//arrange
	char program[] = { ASL_ACC };
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, expected_a);
	assert_flag_c(&state, expected_c);

	//cleanup
	test_cleanup(&state);
}

void test_asl_multiple() {
	test_ASL_ACC(/* A */ 0x0, /* Result*/ 0x0, /* C */ 0);
	test_ASL_ACC(/* A */ 0x1, /* Result*/ 0x2, /* C */ 0);
	test_ASL_ACC(/* A */ 0x1F, /* Result*/ 0x3E, /* C */ 0);
	test_ASL_ACC(/* A */ 0x7F, /* Result*/ 0xFE, /* C */ 0);
	test_ASL_ACC(/* A */ 0x80, /* Result*/ 0x0, /* C */ 1);
	test_ASL_ACC(/* A */ 0x81, /* Result*/ 0x2, /* C */ 1);
	test_ASL_ACC(/* A */ 0xFF, /* Result*/ 0xFE, /* C */ 1);
}

// ROR

void test_ROR_ACC(byte a, byte c, byte expected_a, byte expected_c, byte expected_n, byte expected_z) {
	State6502 state = create_blank_state();
	state.a = a;
	state.flags.c = c;

	//arrange
	char program[] = { ROR_ACC };
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, expected_a);
	assert_flag_c(&state, expected_c);
	assert_flag_n(&state, expected_n);
	assert_flag_z(&state, expected_z);

	//cleanup
	test_cleanup(&state);
}

void test_ror_multiple() {
	test_ROR_ACC(/*A*/ 0x00, /*C*/ 0, /* Result */ 0x00, /* C */ 0, /* N */ 0, /*Z*/ 1);
	test_ROR_ACC(/*A*/ 0x01, /*C*/ 0, /* Result */ 0x00, /* C */ 1, /* N */ 0, /*Z*/ 1);
	test_ROR_ACC(/*A*/ 0x02, /*C*/ 0, /* Result */ 0x01, /* C */ 0, /* N */ 0, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0x40, /*C*/ 0, /* Result */ 0x20, /* C */ 0, /* N */ 0, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0x80, /*C*/ 0, /* Result */ 0x40, /* C */ 0, /* N */ 0, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0x81, /*C*/ 0, /* Result */ 0x40, /* C */ 1, /* N */ 0, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0xC0, /*C*/ 0, /* Result */ 0x60, /* C */ 0, /* N */ 0, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0xFE, /*C*/ 0, /* Result */ 0x7F, /* C */ 0, /* N */ 0, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0xFF, /*C*/ 0, /* Result */ 0x7F, /* C */ 1, /* N */ 0, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0x00, /*C*/ 1, /* Result */ 0x80, /* C */ 0, /* N */ 1, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0x01, /*C*/ 1, /* Result */ 0x80, /* C */ 1, /* N */ 1, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0x02, /*C*/ 1, /* Result */ 0x81, /* C */ 0, /* N */ 1, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0x40, /*C*/ 1, /* Result */ 0xA0, /* C */ 0, /* N */ 1, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0x80, /*C*/ 1, /* Result */ 0xC0, /* C */ 0, /* N */ 1, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0x81, /*C*/ 1, /* Result */ 0xC0, /* C */ 1, /* N */ 1, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0xC0, /*C*/ 1, /* Result */ 0xE0, /* C */ 0, /* N */ 1, /*Z*/ 0);
	test_ROR_ACC(/*A*/ 0xFF, /*C*/ 1, /* Result */ 0xFF, /* C */ 1, /* N */ 1, /*Z*/ 0);
}

/////////////////////

typedef void fp();
fp* tests_lda[] = { test_LDA_IMM, test_LDA_IMM_zero, test_LDA_ZP, test_LDA_ZPX, test_LDA_ZPX_wraparound, test_LDA_ABS, test_LDA_ABSX, test_LDA_ABSY, test_LDA_INDX, test_LDA_INDY, test_LDA_INDX_wraparound, test_LDA_INDY_wraparound };
fp* tests_ora[] = { test_ORA_IMM, test_ORA_ZP, test_ORA_ZPX, test_ORA_ABS, test_ORA_ABSX, test_ORA_ABSY, test_ORA_INDX, test_ORA_INDY, test_ORA_IMM_Z };
fp* tests_and[] = { test_AND_IMM, test_AND_ZP, test_AND_ZPX, test_AND_ABS, test_AND_ABSX, test_AND_ABSY, test_AND_INDX, test_AND_INDY, test_AND_IMM_Z };
fp* tests_ldx[] = { test_LDX_IMM, test_LDX_IMM_zero, test_LDX_ZP, test_LDX_ZPY, test_LDX_ABS, test_LDX_ABSY };
fp* tests_ldy[] = { test_LDY_IMM, test_LDY_IMM_zero, test_LDY_ZP, test_LDY_ZPX, test_LDY_ABS, test_LDY_ABSX };
fp* tests_stx[] = { test_STX_ZP, test_STX_ZPY, test_STX_ABS };
fp* tests_sty[] = { test_STY_ZP, test_STY_ZPX, test_STY_ABS };
fp* tests_inx_iny_dex_dey[] = { test_DEX, test_DEX_wraparound, test_DEY, test_DEY_wraparound, test_INX, test_INX_wraparound, test_INY, test_INY_wraparound };
fp* tests_txa_etc[] = { test_TXA, test_TAX, test_TYA, test_TAY };
fp* tests_inc_dec[] = { test_INC_ZP, test_INC_ZP_multiple, test_INC_ZP_wraparound, test_INC_ZPX, test_INC_ABS, test_INC_ABSX, test_DEC_ZP, test_DEC_ZP_wraparound };
fp* tests_flags[] = { test_CLC, test_SEC, test_CLD, test_SED, test_SEI, test_CLI, test_CLV };
fp* tests_eor[] = { test_EOR_IMM, test_EOR_ZP, test_EOR_ZPX, test_EOR_ABS, test_EOR_ABSX, test_EOR_ABSY, test_EOR_INDX, test_EOR_INDY, test_EOR_IMM_Z };
fp* tests_sta[] = { test_STA_ZP, test_STA_ZPX, test_STA_ABS, test_STA_ABSX, test_STA_ABSY, test_STA_INDX, test_STA_INDY };
fp* tests_pha_pla[] = { test_PHA, test_PLA, test_PLA_N, test_PLA_Z, test_PHA_PLA };
fp* tests_txs_tsx[] = { test_TXS, test_TSX, test_TXS_Z };
fp* tests_php_plp[] = { test_PHP, test_PHP_no_flags, test_PLP, test_PLP2 };
fp* tests_jmp[] = { test_JMP, test_JMP_IND, test_JMP_IND_wrap };
fp* tests_cmp[] = { test_CMP_ABS_equal, test_CMP_ABS_greater, test_CMP_ABS_greater_2, test_CMP_ABS_less_than, test_CPX_ABS, test_CPY_ABS };
fp* tests_sbc[] = { test_SBC_IMM_multiple };
fp* tests_adc[] = { test_ADC_IMM_multiple };
fp* tests_bit[] = { test_BIT_multiple };
fp* tests_jsr_rts[] = { test_JSR, test_JSR_RTS, test_RTS };
fp* tests_brk[] = { test_BRK };
fp* tests_branch[] = { test_branching_multiple };
fp* tests_rti[] = { test_RTI };
fp* tests_asl[] = { test_asl_multiple };
fp* tests_ror[] = { test_ror_multiple };

#define RUN(suite) run_suite(suite, sizeof(suite)/sizeof(fp*))

void run_suite(fp * *suite, int size) {
	for (int i = 0; i < size; i++)
	{
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		suite[i]();
	}
}

void run_tests() {
	RUN(tests_ror);
	RUN(tests_asl);
	RUN(tests_rti);
	RUN(tests_branch);
	RUN(tests_sbc);
	RUN(tests_brk);
	RUN(tests_jsr_rts);
	RUN(tests_bit);
	RUN(tests_adc);
	RUN(tests_ora);
	RUN(tests_and);
	RUN(tests_lda);
	RUN(tests_ldx);
	RUN(tests_ldy);
	RUN(tests_stx);
	RUN(tests_sty);
	RUN(tests_inx_iny_dex_dey);
	RUN(tests_txa_etc);
	RUN(tests_inc_dec);
	RUN(tests_flags);
	RUN(tests_eor);
	RUN(tests_sta);
	RUN(tests_pha_pla);
	RUN(tests_txs_tsx);
	RUN(tests_jmp);
	RUN(tests_php_plp);
	RUN(tests_cmp);
	printf("All tests succeeded.\n");
}