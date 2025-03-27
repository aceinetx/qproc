#pragma once

/* clang-format off */
#define OPCODE_R(name) name##_R0, name##_R1, name##_R2, name##_R3, name##_R4, name##_R5, name##_R6, name##_R7, name##_R8, name##_R9, name##_R10, name##_R11, name##_R12, name##_SP, name##_BP, name##_IP
#define JMP_CONST 0xff
/* clang-format on */

/* size specifiers */
enum { SS_DWORD, SS_WORD, SS_BYTE };

/* interrupts */
enum { INT_PUTC = 1, INT_GETC, INT_GETS };

enum {
	OPCODE_R(MOV),
	OPCODE_R(MOVI),

	HLT,
	NOP,

	OPCODE_R(LOD),

	PUSH,
	PUSHI,

	POP,

	OPCODE_R(CMP),

	BE,
	BNE,
	BL,
	BG,
	BLE,
	BGE,

	OPCODE_R(STR),

	OPCODE_R(ADD),
	OPCODE_R(SUB),

	CALL,
	CALLI,

	QDB,

	OPCODE_R(MUL),
	OPCODE_R(DIV),

	SWI,

	OPCODE_R(OR),
	OPCODE_R(XOR),
	OPCODE_R(AND)
};

#undef OPCODE_R
