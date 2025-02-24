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

Exception JlInstruction::execute(VM *vm) {
  if (vm->flags.CF == 1) {
    return JmpInstruction::execute(vm);
  }
  return Exception::NONE;
}

Exception JgInstruction::execute(VM *vm) {
  if (vm->flags.CF == 0 && vm->flags.ZF == 0) {
    return JmpInstruction::execute(vm);
  }
  return Exception::NONE;
}

Exception JleInstruction::execute(VM *vm) {
  if (vm->flags.CF == 1 || vm->flags.ZF == 1) {
    return JmpInstruction::execute(vm);
  }
  return Exception::NONE;
}

Exception JgeInstruction::execute(VM *vm) {
  if (vm->flags.CF == 0 || vm->flags.ZF == 1) {
    return JmpInstruction::execute(vm);
  }
  return Exception::NONE;
}
