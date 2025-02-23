#include "fs.h"

#include <fstream>
#include <istream>
#include <sstream>
#include <string>

#include "bytearray.h"
#include "result.h"

Result<std::string, std::string> readFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file) {
    return Result<std::string, std::string>::error("Reading failed");
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  return Result<std::string, std::string>::success(buffer.str());
}

Result<ByteArray, std::string> readFileBytes(std::string filename) {
  std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
  if (!ifs) {
    return Result<ByteArray, std::string>::error("Reading failed");
  }
  std::ifstream::pos_type pos = ifs.tellg();

  ByteArray result(pos);

  ifs.seekg(0, std::ios::beg);
  ifs.read((char *)&result[0], pos);

  return Result<ByteArray, std::string>::success(result);
}