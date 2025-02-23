#include "push.h"

#include <string.h>

Exception PushcInstruction::execute(VM* vm) {
	if (vm->registers.sp < 4)
		return Exception::MEMORY_NOT_IN_BOUNDS;
	memcpy(&vm->memory[vm->registers.sp - 4], &source, 4);
	vm->registers.sp -= 4;
	return Exception::NONE;
}

Exception PushInstruction::execute(VM* vm) {
	if (vm->registers.sp < 4)
		return Exception::MEMORY_NOT_IN_BOUNDS;
	memcpy(&vm->memory[vm->registers.sp - 4], source, 4);
	vm->registers.sp -= 4;
	return Exception::NONE;
}