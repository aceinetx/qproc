#pragma once
#include "instruction.h"

class StrInstruction : public Instruction {
public:
	dword *dest;
	dword *source;

	virtual Exception execute(VM *vm) = 0;
};

class StrDwordInstruction : public StrInstruction {
public:
	Exception execute(VM *vm);
};

class StrWordInstruction : public StrInstruction {
public:
	Exception execute(VM *vm);
};

class StrByteInstruction : public StrInstruction {
public:
	Exception execute(VM *vm);
};
