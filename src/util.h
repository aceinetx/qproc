#include <string>
#include <vector>

#include "qvm.h"

std::vector<std::string> split(std::string s, std::string delimiter);

std::string replaceAll(const std::string &str, const std::string &from,
											 const std::string &to);

bool isHexString(std::string s);
bool isInteger(std::string s);

int convertQEndian(std::vector<byte> bytes);
int convertQEndian(byte *bytes);
std::vector<byte> convertQEndian(int n);

std::string args_shift(int *argc, char ***argv);