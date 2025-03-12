#include "assembler.h"
#include <regex>
#include <registers.h>

#include <format>

#include <opcodes.h>
#include <util.h>

std::vector<std::string> split(std::string s, std::string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

std::string replaceAll(const std::string &str, const std::string &from,
											 const std::string &to) {
	if (from.empty())
		return str; // Avoid empty substring case

	std::string result = str; // Create a copy of the original string
	size_t start_pos = 0;
	while ((start_pos = result.find(from, start_pos)) != std::string::npos) {
		result.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Move past the replaced part
	}
	return result; // Return the modified string
}

bool isHexString(std::string s) {
	return std::regex_match(s, std::regex("^(0x|0X)?[a-fA-F0-9]+$")) &&
				 (s.starts_with("0x") || s.starts_with("0X"));
}

bool isInteger(std::string s) {
	for (char c : s)
		if (!(c >= '0' && c <= '9'))
			return false;
	return true;
}

int convertQEndian(std::vector<byte> bytes) {
	dword value = 0;
	for (dword i = 0; i < 4; ++i) {
		value |= (static_cast<dword>(bytes[i]) << (i * 8));
	}
	return value;
}

int convertQEndian(byte *bytes) {
	dword value = 0;
	for (dword i = 0; i < 4; ++i) {
		value |= (static_cast<dword>(bytes[i]) << (i * 8));
	}
	return value;
}

std::vector<byte> convertQEndian(int n) {
	std::vector<byte> bytes(sizeof(dword));
	for (size_t i = 0; i < bytes.size(); ++i) {
		bytes[i] = (n >> (i * 8)) & 0xFF;
	}
	return bytes;
}

unsigned stou(std::string const &str, size_t *idx = 0, int base = 10) {
	unsigned long result = std::stoul(str, idx, base);
	if (result > std::numeric_limits<unsigned>::max()) {
		throw std::out_of_range("stou");
	}
	return result;
}

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

	if (operand == ".$") {
		for (byte b : convertQEndian(addr)) {
			output.push_back(b);
		}
		return Result<ByteArray, std::string>::success(output);
	}

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

						for (byte b : convertQEndian(stou(instruction[2], 0, base))) {
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

						for (byte b : convertQEndian(stou(instruction[1], 0, base))) {
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
			} else if (instruction[0] == "bmp") { // jmp
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

						for (byte b : convertQEndian(stou(instruction[1], 0, base))) {
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

				if (instruction[0] == "be") {
					output.push_back(BE);
				} else if (instruction[0] == "bne") {
					output.push_back(BNE);
				} else if (instruction[0] == "bl") {
					output.push_back(BL);
				} else if (instruction[0] == "bg") {
					output.push_back(BG);
				} else if (instruction[0] == "ble") {
					output.push_back(BLE);
				} else if (instruction[0] == "bge") {
					output.push_back(BGE);
				} else {
					return Result<ByteArray, std::string>::error(std::format(
							"[{}] Invalid instruction, excepted a branch kind instruction",
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

						for (byte b : convertQEndian(stou(instruction[1], 0, base))) {
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

						output.push_back(convertQEndian(stou(instruction[1], 0, base))[0]);
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

						auto conv = convertQEndian(stou(instruction[1], 0, base));

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

						auto conv = convertQEndian(stou(instruction[1], 0, base));

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
			} else if (instruction[0] == "add") { // add
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

				output.push_back(left.get_success().value() + ADD_R0);
				output.push_back(right.get_success().value());
				addr += 0x02;
			} else if (instruction[0] == "sub") { // sub
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

				output.push_back(left.get_success().value() + SUB_R0);
				output.push_back(right.get_success().value());
				addr += 0x02;
			} else if (instruction[0] == "call") { // call
				if (instruction.size() < 1) {
					return Result<ByteArray, std::string>::error(
							std::format("[{}] Invalid operand count", line_num));
				}

				Result<byte, std::string> left = processOperand(instruction[1]);

				if (left.is_error())
					return Result<ByteArray, std::string>::error(
							std::format("[{}] {}", line_num, left.get_error().value()));

				output.push_back(CALL);
				output.push_back(left.get_success().value());
				addr += 0x02;
			} else if (instruction[0] == "qdb") { // qdb
				output.push_back(QDB);
				addr += 0x01;
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
