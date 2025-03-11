#pragma once

#define OPCODE_R(name)                                                         \
	name##_R0, name##_R1, name##_R2, name##_R3, name##_R4, name##_SP, name##_BP, \
			name##_IP

typedef enum Opcodes {
	OPCODE_R(MOV),
	OPCODE_R(MOVC),

	HLT,
	NOP,

	OPCODE_R(LOD),

	PUSH,
	PUSHC,

	POP,

	OPCODE_R(CMP),

	JE,
	JNE,
	JL,
	JG,
	JLE,
	JGE,

	OPCODE_R(STR),

	OPCODE_R(ADD),
	OPCODE_R(SUB),

	CALL,
	CALLC,

	QDB,
} Opcodes;
