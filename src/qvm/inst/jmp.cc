#include "jmp.h"

#include <string.h>

Exception JmpInstruction::execute(VM *vm) {
  vm->registers.ip = dest;
  return Exception::NONE;
}

Exception JeInstruction::execute(VM *vm) {
  if (vm->flags.ZF == 1) {
    return JmpInstruction::execute(vm);
  }
  return Exception::NONE;
}
