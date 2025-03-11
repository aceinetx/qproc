#include "add.h"

#include <string.h>

Exception AddInstruction::execute(VM *vm) {
	*dest += *source;
	return Exception::NONE;
}
