#pragma once
#include "instruction.h"

class PushcInstruction : public Instruction {
public:
	dword source;

	Exception execute(VM *vm);
};

class PushInstruction : public Instruction {
public:
	dword *source;

	Exception execute(VM *vm);
};
