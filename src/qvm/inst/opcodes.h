#pragma once

typedef enum Opcodes {
	MOV_R0,
	MOV_R1,
	MOV_R2,
	MOV_R3,
	MOV_R4,
	MOV_SP,
	MOV_BP,
	MOV_IP,

	MOVC_R0,
	MOVC_R1,
	MOVC_R2,
	MOVC_R3,
	MOVC_R4,
	MOVC_SP,
	MOVC_BP,
	MOVC_IP,

	HLT,
	NOP,

	LOD_R0,
	LOD_R1,
	LOD_R2,
	LOD_R3,
	LOD_R4,
	LOD_SP,
	LOD_BP,
	LOD_IP,

	PUSH,
	PUSHC,

	POP,

	CMP_R0,
	CMP_R1,
	CMP_R2,
	CMP_R3,
	CMP_R4,
	CMP_SP,
	CMP_BP,
	CMP_IP,

	JE,
	JNE,
	JL,
	JG,
	JLE,
	JGE,

	STR_R0,
	STR_R1,
	STR_R2,
	STR_R3,
	STR_R4,
	STR_SP,
	STR_BP,
	STR_IP,
} Opcodes;
