#pragma once
#include "instruction.h"
#include "lod.h"

class LodWordInstruction : public LodInstruction {
public:
	Exception execute(VM *vm);
};
