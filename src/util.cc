#include "util.h"

#include <regex>

#include "assert.h"

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
		return str;	 // Avoid empty substring case

	std::string result = str;	 // Create a copy of the original string
	size_t start_pos = 0;
	while ((start_pos = result.find(from, start_pos)) != std::string::npos) {
		result.replace(start_pos, from.length(), to);
		start_pos += to.length();	 // Move past the replaced part
	}
	return result;	// Return the modified string
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

std::string args_shift(int *argc, char ***argv) {
	assert(*argc > 0 && "argc <= 0");
	--(*argc);
	return *(*argv)++;
}