#include "sub.h"

#include <string.h>

Exception SubInstruction::execute(VM *vm) {
	*dest -= *source;
	return Exception::NONE;
}
