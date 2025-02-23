#pragma once
#include <result.h>

#include "qvm.h"
#include <string>

Result<std::string, std::string> readFile(const std::string &filename);
Result<ByteArray, std::string> readFileBytes(std::string filename);
