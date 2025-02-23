#include "lod_dword.h"

Exception LodDwordInstruction::execute(VM *vm) {
  *dest = *(dword *)(&vm->memory[*source]);
  return Exception::NONE;
}