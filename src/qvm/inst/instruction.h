#pragma once
#include "opcodes.h"
#include "qvm.h"
#include "vm.h"

enum class Exception;
class VM;

/*
 * Base class for a instruction
 */
class Instruction {
public:
	virtual Exception execute(VM *vm) = 0;
	virtual ~Instruction() {
	}
};
