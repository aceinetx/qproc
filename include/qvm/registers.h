#pragma once
#include <qvm.h>

enum { R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, SP, BP, IP };

typedef struct {
	dword r0;
	dword r1;
	dword r2;
	dword r3;
	dword r4;
	dword r5;
	dword r6;
	dword r7;
	dword r8;
	dword r9;
	dword r10;
	dword r11;
	dword r12;
	dword sp;
	dword bp;
	dword ip;
} registers;
