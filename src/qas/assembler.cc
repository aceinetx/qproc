#include "assembler.h"

#include <format>

#include "opcodes.h"
#include "register.h"
#include "util.h"

Assembler::Assembler(std::string code) {
	this->code = code;
	this->addr = 0;
}

Result<byte, std::string> Assembler::processOperand(std::string operand) {
	if (operand == "r0") {
		return Result<byte, std::string>::success(R0);
	} else if (operand == "r1") {
		return Result<byte, std::string>::success(R1);
	} else if (operand == "r2") {
		return Result<byte, std::string>::success(R2);
	} else if (operand == "r3") {
		return Result<byte, std::string>::success(R3);
	} else if (operand == "r4") {
		return Result<byte, std::string>::success(R4);
	} else if (operand == "sp") {
		return Result<byte, std::string>::success(SP);
	} else if (operand == "bp") {
		return Result<byte, std::string>::success(BP);
	} else if (operand == "ip") {
		return Result<byte, std::string>::success(IP);
	}
	return Result<byte, std::string>::error("Invalid operand");
}

Result<ByteArray, std::string> Assembler::doLabel(std::string operand) {
	if (inPreprocessor)
		return Result<ByteArray, std::string>::success(output);

	if (!labels.count(operand)) { // label does not exist
		return Result<ByteArray, std::string>::error(
				std::format("Undefined label"));
	}
	for (byte b : convertQEndian(labels[operand])) {
		output.push_back(b);
	}
	return Result<ByteArray, std::string>::success(output);
}

Result<ByteArray, std::string> Assembler::assemble() {
	output.clear();

	inPreprocessor = true;
	for (;;) {
		if (!inPreprocessor) {
			printf("Preprocessed, doing compilation now\n");
		}

		line_num = -1;
		addr = 0x00;
		for (std::string &line : split(code, "\n")) {
			line_num++;
			if (line.starts_with(';')) // skip comments
				continue;

			// Filter unnecessary characters
			while (line.starts_with(' '))
				line.erase(line.begin());
			while (line.starts_with('\t'))
				line.erase(line.begin());
			while (line.ends_with(' '))
				line.pop_back();
			while (line.ends_with('\t'))
				line.pop_back();

			if (line == "\n" || line.empty())
				continue;

			if (line.starts_with('.')) {
				if (!inPreprocessor)
					continue;
				labels[line] = addr;
				// printf("declared label %s\n", line.c_str());
				continue;
			}

			std::vector<std::string> instruction = split(line, " ");
			if (instruction[0] == "mov") { // mov
				if (instruction.size() < 3) {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid operand count", line_num));
				}

				if (isHexString(instruction[2]) || isInteger(instruction[2]) ||
						instruction[2][0] == '.') { // movc
					Result<byte, std::string> left = processOperand(instruction[1]);
					if (left.is_error())
						return Result<ByteArray, std::string>::error(
								std::format("[{}] {}", line_num, left.get_error().value()));

					output.push_back(MOVC_R0 + left.get_success().value());

					if (instruction[2][0] == '.') { // if a label
						auto result = doLabel(instruction[2]);
						if (result.is_error()) {
							return Result<ByteArray, std::string>::error(
									std::format("[{}] {}", line_num, result.get_error().value()));
						}
					} else {
						int base = 16;
						if (isInteger(instruction[2]))
							base = 10;

						for (byte b : convertQEndian(std::stoi(instruction[2], 0, base))) {
							output.push_back(b);
						}
					}

					addr += 0x05;
				} else {
					Result<byte, std::string> left = processOperand(instruction[1]);
					Result<byte, std::string> right = processOperand(instruction[2]);

					if (left.is_error())
						return Result<ByteArray, std::string>::error(
								std::format("[{}] {}", line_num, left.get_error().value()));

					if (right.is_error())
						return Result<ByteArray, std::string>::error(
								std::format("[{}] {}", line_num, right.get_error().value()));

					output.push_back(MOV_R0 + left.get_success().value());
					output.push_back(right.get_success().value());
					addr += 0x02;
				}
			} else if (instruction[0] == "push") { // push
				if (instruction.size() < 2) {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid operand count", line_num));
				}

				output.push_back(PUSH);

				if (isHexString(instruction[1]) || isInteger(instruction[1]) ||
						instruction[1][0] == '.') {		// pushc
					if (instruction[1][0] == '.') { // if a label
						auto result = doLabel(instruction[1]);
						if (result.is_error()) {
							return Result<ByteArray, std::string>::error(
									std::format("[{}] {}", line_num, result.get_error().value()));
						}
					} else {
						int base = 16;
						if (isInteger(instruction[1]))
							base = 10;

						for (byte b : convertQEndian(std::stoi(instruction[1], 0, base))) {
							output.push_back(b);
						}
					}

					addr += 0x02;
				} else {
					Result<byte, std::string> op = processOperand(instruction[1]);

					if (op.is_error())
						return Result<ByteArray, std::string>::error(
								std::format("[{}] {}", line_num, op.get_error().value()));

					output.push_back(op.get_success().value());
					addr += 0x02;
				}
			} else if (instruction[0] == "cmp") { // cmp
				if (instruction.size() < 2) {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid operand count", line_num));
				}

				Result<byte, std::string> left = processOperand(instruction[1]);
				Result<byte, std::string> right = processOperand(instruction[2]);

				if (left.is_error())
					return Result<ByteArray, std::string>::error(
							std::format("[{}] {}", line_num, left.get_error().value()));
				if (right.is_error())
					return Result<ByteArray, std::string>::error(
							std::format("[{}] {}", line_num, right.get_error().value()));

				output.push_back(left.get_success().value() + CMP_R0);
				output.push_back(right.get_success().value());
				addr += 0x02;
			} else if (instruction[0] == "hlt") { // hlt
				output.push_back(HLT);
				addr += 0x01;
			} else if (instruction[0] == "nop") { // nop
				output.push_back(NOP);
				addr += 0x01;
			} else if (instruction[0] == "pop") { // pop
				if (instruction.size() < 2) {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid operand count", line_num));
				}

				output.push_back(POP);
				Result<byte, std::string> op = processOperand(instruction[1]);

				if (op.is_error())
					return Result<ByteArray, std::string>::error(
							std::format("[{}] {}", line_num, op.get_error().value()));

				output.push_back(op.get_success().value());
				addr += 0x02;
			} else if (instruction[0] == "lod") { // lod
				if (instruction.size() < 4) {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid operand count", line_num));
				}

				Result<byte, std::string> left = processOperand(instruction[3]);
				Result<byte, std::string> right = processOperand(instruction[1]);

				if (left.is_error())
					return Result<ByteArray, std::string>::error(
							std::format("[{}] {}", line_num, left.get_error().value()));
				if (right.is_error())
					return Result<ByteArray, std::string>::error(
							std::format("[{}] {}", line_num, right.get_error().value()));

				output.push_back(LOD_R0 + left.get_success().value());
				if (instruction[2] == "dword") {
					output.push_back(0x00);
				} else if (instruction[2] == "word") {
					output.push_back(0x01);
				} else if (instruction[2] == "byte") {
					output.push_back(0x02);
				} else {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid size specifier", line_num));
				}

				output.push_back(right.get_success().value());
				addr += 0x03;
			} else if (instruction[0] == "jmp") { // jmp
				if (instruction.size() < 2) {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid operand count", line_num));
				}

				if (isHexString(instruction[1]) || isInteger(instruction[1]) ||
						instruction[1][0] == '.') { // jmp constant

					output.push_back(MOVC_IP);

					if (instruction[1][0] == '.') { // if a label
						auto result = doLabel(instruction[1]);
						if (result.is_error()) {
							return Result<ByteArray, std::string>::error(
									std::format("[{}] {}", line_num, result.get_error().value()));
						}
					} else {
						int base = 16;
						if (isInteger(instruction[1]))
							base = 10;

						for (byte b : convertQEndian(std::stoi(instruction[1], 0, base))) {
							output.push_back(b);
						}
					}

					addr += 0x05;
				} else {
					Result<byte, std::string> left = processOperand(instruction[1]);

					if (left.is_error())
						return Result<ByteArray, std::string>::error(
								std::format("[{}] {}", line_num, left.get_error().value()));

					output.push_back(MOV_IP);
					output.push_back(left.get_success().value());
					addr += 0x02;
				}
			} else if (instruction[0][0] == 'j') { // jmps
				if (instruction.size() < 2) {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid operand count", line_num));
				}

				if (instruction[0] == "je") {
					output.push_back(JE);
				} else if (instruction[0] == "jne") {
					output.push_back(JNE);
				} else if (instruction[0] == "jl") {
					output.push_back(JL);
				} else if (instruction[0] == "jg") {
					output.push_back(JG);
				} else if (instruction[0] == "jle") {
					output.push_back(JLE);
				} else if (instruction[0] == "jge") {
					output.push_back(JGE);
				} else {
					return Result<ByteArray, std::string>::error(std::format(
							"[{}] Invalid instruction, excepted a jump kind instruction",
							line_num));
				}

				if (isHexString(instruction[1]) || isInteger(instruction[1]) ||
						instruction[1][0] == '.') { // constant
					output.push_back(0xff);
					if (instruction[1][0] == '.') { // if a label
						auto result = doLabel(instruction[1]);
						if (result.is_error()) {
							return Result<ByteArray, std::string>::error(
									std::format("[{}] {}", line_num, result.get_error().value()));
						}
					} else {
						int base = 16;
						if (isInteger(instruction[1]))
							base = 10;

						for (byte b : convertQEndian(std::stoi(instruction[1], 0, base))) {
							output.push_back(b);
						}
					}

					addr += 0x06;
				} else {
					Result<byte, std::string> op = processOperand(instruction[1]);

					if (op.is_error())
						return Result<ByteArray, std::string>::error(
								std::format("[{}] {}", line_num, op.get_error().value()));

					output.push_back(op.get_success().value());
					addr += 0x02;
				}
			} else if (instruction[0] == "str") { // str
				if (instruction.size() < 4) {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid operand count", line_num));
				}

				Result<byte, std::string> left = processOperand(instruction[2]);
				Result<byte, std::string> right = processOperand(instruction[3]);

				if (left.is_error())
					return Result<ByteArray, std::string>::error(
							std::format("[{}] {}", line_num, left.get_error().value()));
				if (right.is_error())
					return Result<ByteArray, std::string>::error(
							std::format("[{}] {}", line_num, right.get_error().value()));

				output.push_back(STR_R0 + left.get_success().value());
				if (instruction[1] == "dword") {
					output.push_back(0x00);
				} else if (instruction[1] == "word") {
					output.push_back(0x01);
				} else if (instruction[1] == "byte") {
					output.push_back(0x02);
				} else {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid size specifier", line_num));
				}

				output.push_back(right.get_success().value());
				addr += 0x03;
			} else if (instruction[0] == "byte") { // byte
				if (instruction.size() < 2) {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid operand count", line_num));
				}

				if (isHexString(instruction[1]) || isInteger(instruction[1]) ||
						instruction[1][0] == '.') {
					if (instruction[1][0] == '.') { // if a label
						return Result<ByteArray, std::string>::error(
								std::format("[{}] Label cannot fit into a byte", line_num));
					} else {
						int base = 16;
						if (isInteger(instruction[1]))
							base = 10;

						output.push_back(
								convertQEndian(std::stoi(instruction[1], 0, base))[0]);
					}

					addr += 0x01;
				} else {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Excepted a lvalue", line_num));
				}
			} else if (instruction[0] == "word") { // word
				if (instruction.size() < 2) {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid operand count", line_num));
				}

				if (isHexString(instruction[1]) || isInteger(instruction[1]) ||
						instruction[1][0] == '.') {
					if (instruction[1][0] == '.') { // if a label
						return Result<ByteArray, std::string>::error(
								std::format("[{}] Label cannot fit into a word", line_num));
					} else {
						int base = 16;
						if (isInteger(instruction[1]))
							base = 10;

						auto conv = convertQEndian(std::stoi(instruction[1], 0, base));

						output.push_back(conv[0]);
						output.push_back(conv[1]);
					}

					addr += 0x02;
				} else {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Excepted a lvalue", line_num));
				}
			} else if (instruction[0] == "dword") { // dword
				if (instruction.size() < 2) {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid operand count", line_num));
				}

				if (isHexString(instruction[1]) || isInteger(instruction[1]) ||
						instruction[1][0] == '.') {
					if (instruction[1][0] == '.') { // if a label
						auto result = doLabel(instruction[1]);
						if (result.is_error()) {
							return Result<ByteArray, std::string>::error(
									std::format("[{}] {}", line_num, result.get_error().value()));
						}
					} else {
						int base = 16;
						if (isInteger(instruction[1]))
							base = 10;

						auto conv = convertQEndian(std::stoi(instruction[1], 0, base));

						output.push_back(conv[0]);
						output.push_back(conv[1]);
						output.push_back(conv[2]);
						output.push_back(conv[3]);
					}

					addr += 0x04;
				} else {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Excepted a lvalue", line_num));
				}
			} else {
				return Result<ByteArray, std::string>::error(
						std::format("[{}] Invalid instruction", line_num));
			}
		}
		if (inPreprocessor == false)
			break;
		inPreprocessor = false;
		output.clear();
	}

	return Result<ByteArray, std::string>::success(output);
}

Assembler::~Assembler() = default;
