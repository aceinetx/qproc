#pragma once
#include <string>

#include "bytearray.h"
#include "qvm.h"
#include "result.h"
#include <map>

class Assembler {
public:
  std::string code;
  dword addr;
  std::map<std::string, dword> labels;

  Assembler(std::string code = "");

  Result<byte, std::string> processOperand(std::string operand);
  Result<ByteArray, std::string> assemble();

  ~Assembler();
};
