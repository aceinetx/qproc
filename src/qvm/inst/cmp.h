#pragma once
#include "instruction.h"

class CmpInstruction : public Instruction {
public:
  dword *left;
  dword *right;

  Exception execute(VM *vm);
};
