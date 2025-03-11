#pragma once
#include "instruction.h"

class AddInstruction : public Instruction {
public:
	dword *dest;
	dword *source;

	Exception execute(VM *vm);
};
