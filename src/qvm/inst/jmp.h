#pragma once
#include "instruction.h"

class JmpInstruction : public Instruction {
public:
	dword dest;

	virtual Exception execute(VM *vm);
};

class JeInstruction : public JmpInstruction {
public:
	Exception execute(VM *vm);
};

class JlInstruction : public JmpInstruction {
public:
	Exception execute(VM *vm);
};

class JgInstruction : public JmpInstruction {
public:
	Exception execute(VM *vm);
};

class JleInstruction : public JmpInstruction {
public:
	Exception execute(VM *vm);
};

class JgeInstruction : public JmpInstruction {
public:
	Exception execute(VM *vm);
};
