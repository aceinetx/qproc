#pragma once
#include <string>
#include <vector>

typedef std::vector<unsigned char> ByteArray;
typedef struct QasSettings {
	std::string output_filename;
	std::vector<std::string> sources;
} QasSettings;

extern QasSettings settings;
