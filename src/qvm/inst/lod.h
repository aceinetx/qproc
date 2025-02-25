#pragma once
#include "instruction.h"

class LodInstruction : public Instruction {
public:
	dword *dest;
	dword *source;

	virtual Exception execute(VM *vm) = 0;
};

class LodDwordInstruction : public LodInstruction {
public:
	Exception execute(VM *vm);
};

class LodWordInstruction : public LodInstruction {
public:
	Exception execute(VM *vm);
};

class LodByteInstruction : public LodInstruction {
public:
	Exception execute(VM *vm);
};
