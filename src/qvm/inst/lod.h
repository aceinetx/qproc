#pragma once
#include "instruction.h"

class LodInstruction : public Instruction {
public:
	dword *dest;
	dword *source;

	virtual Exception execute(VM *vm) = 0;
};
