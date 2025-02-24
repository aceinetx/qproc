#pragma once
#include "instruction.h"

class PopInstruction : public Instruction {
public:
	dword *dest;

	Exception execute(VM *vm);
};
