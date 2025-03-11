#include "call.h"
#include "push.h"

#include <string.h>

Exception CallInstruction::execute(VM *vm) {
	PushInstruction push;
	push.source = &vm->registers.ip;
	push.execute(vm);

	vm->registers.ip = *dest;
	return Exception::NONE;
}

Exception CallcInstruction::execute(VM *vm) {
	PushInstruction push;
	push.source = &vm->registers.ip;
	push.execute(vm);

	vm->registers.ip = dest;
	return Exception::NONE;
}
