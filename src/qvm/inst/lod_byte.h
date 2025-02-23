#pragma once
#include <typeinfo>

#include "instruction.h"
#include "lod.h"

class LodByteInstruction : public LodInstruction {
public:
  Exception execute(VM *vm);
};
