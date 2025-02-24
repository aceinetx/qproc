#include "pop.h"

#include <string.h>

Exception PopInstruction::execute(VM *vm) {
  if (vm->registers.sp > MEMORY_SIZE - 4 - 1)
    return Exception::MEMORY_NOT_IN_BOUNDS;
  *dest = vm->memory[vm->registers.sp];
  vm->registers.sp += 4;
  return Exception::NONE;
}
