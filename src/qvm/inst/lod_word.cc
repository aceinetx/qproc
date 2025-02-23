#include "lod_word.h"

Exception LodWordInstruction::execute(VM* vm) {
	*dest = *(word*) (&vm->memory[*source]);
	return Exception::NONE;
}