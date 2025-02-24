#include "cmp.h"

#include <string.h>

Exception CmpInstruction::execute(VM *vm) {
  vm->flags.ZF = 0;
  if (*left == *right) {
    vm->flags.ZF = 1;
  }
  vm->flags.CF = 0;
  if (*left < *right) {
    vm->flags.CF = 1;
  }
  return Exception::NONE;
}
