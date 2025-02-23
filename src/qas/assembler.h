#pragma once
#include <string>

#include "bytearray.h"
#include "qvm.h"
#include "result.h"

class Assembler {
public:
  std::string code;
  dword addr;

  Assembler(std::string code = "");

  Result<byte, std::string> processOperand(std::string operand);
  Result<ByteArray, std::string> assemble();

  ~Assembler();
};