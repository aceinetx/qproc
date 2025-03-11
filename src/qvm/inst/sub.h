#pragma once
#include "instruction.h"

class SubInstruction : public Instruction {
public:
	dword *dest;
	dword *source;

	Exception execute(VM *vm);
};
