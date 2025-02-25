#include "lod.h"

Exception LodDwordInstruction::execute(VM *vm) {
	*dest = *(dword *) (&vm->memory[*source]);
	return Exception::NONE;
}

Exception LodWordInstruction::execute(VM *vm) {
	*dest = *(word *) (&vm->memory[*source]);
	return Exception::NONE;
}

Exception LodByteInstruction::execute(VM *vm) {
	*dest = *(byte *) (&vm->memory[*source]);
	return Exception::NONE;
}
