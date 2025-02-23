#include "lod_byte.h"

Exception LodByteInstruction::execute(VM *vm) {
  *dest = *(byte *)(&vm->memory[*source]);
  return Exception::NONE;
}