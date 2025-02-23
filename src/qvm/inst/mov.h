#pragma once
#include "instruction.h"

class MovInstruction : public Instruction {
public:
	dword* dest;
	dword* source;

	Exception execute(VM* vm);
};

class MovcInstruction : public Instruction {
public:
	dword* dest;
	dword source;

	Exception execute(VM* vm);
};