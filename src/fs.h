#pragma once
#include <result.h>

#include <string>

#include "bytearray.h"

Result<std::string, std::string> readFile(const std::string& filename);
Result<ByteArray, std::string> readFileBytes(std::string filename);