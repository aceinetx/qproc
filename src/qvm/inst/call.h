#pragma once
#include "instruction.h"

class CallInstruction : public Instruction {
public:
	dword *dest;

	Exception execute(VM *vm);
};

class CallcInstruction : public Instruction {
public:
	dword dest;

	Exception execute(VM *vm);
};
