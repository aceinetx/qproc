#include "str.h"

#include <string.h>

Exception StrDwordInstruction::execute(VM *vm) {
	memcpy(&vm->memory[*dest], source, 4);
	return Exception::NONE;
}

Exception StrWordInstruction::execute(VM *vm) {
	memcpy(&vm->memory[*dest], source, 2);
	return Exception::NONE;
}

Exception StrByteInstruction::execute(VM *vm) {
	memcpy(&vm->memory[*dest], source, 1);
	return Exception::NONE;
}
