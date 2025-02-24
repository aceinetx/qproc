#pragma once
#include <map>
#include <string>

#include "qvm.h"
#include "result.h"

class Assembler {
private:
	ByteArray output;
	bool inPreprocessor;
	int line_num;

public:
	std::string code;
	dword addr;
	std::map<std::string, dword> labels;

	Result<ByteArray, std::string> doLabel(std::string operand);

	Assembler(std::string code = "");

	Result<byte, std::string> processOperand(std::string operand);
	Result<ByteArray, std::string> assemble();

	~Assembler();
};
