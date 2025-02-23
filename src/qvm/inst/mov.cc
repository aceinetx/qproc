#include "mov.h"

Exception MovcInstruction::execute(VM *vm) {
  *dest = source;
  (void)vm;
  return Exception::NONE;
}

Exception MovInstruction::execute(VM *vm) {
  *dest = *source;
  (void)vm;
  return Exception::NONE;
}