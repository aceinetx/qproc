#pragma once
#include <string>
#include <vector>

#include "instruction.h"
#include "qvm.h"
#include "register.h"

enum class Exception {
	NONE,
	MEMORY_NOT_IN_BOUNDS,
	ILLEGAL_REGISTER,
};

enum class DebugState {
	NONE,
	EXCEPTION,
};

class Instruction;
typedef struct InstConvResult {
	Instruction *output;
	bool success;
	std::string disassembly;
} InstConvResult;

class VM {
public:
	registers_t registers;
	byte *memory;
	word exception;
	DebugState debug_state;
	bool log_instructions;

	struct flags_t {
		byte CF;
		byte ZF;
	} flags;

	std::vector<byte> getForward(int n);
	InstConvResult convertIntoInstruction();
	dword *getRegisterFromIndex(int index);
	std::string getRegisterNameFromIndex(int index);
	void execute();
	void fprintState(FILE *descriptor);
	bool handleException(Exception exception);

	VM();
	~VM();
};
