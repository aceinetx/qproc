#pragma once
#include "qvm.h"
#include "reg_operands.h"

#define REGISTERS_COUNT 7
typedef struct registers_t {
  // general purpose registers
  dword r0;
  dword r1;
  dword r2;
  dword r3;
  dword r4;

  dword sp;
  dword bp;
  dword ip;
} registers_t;
