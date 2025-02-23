#pragma once
#include <string>
#include <vector>

typedef struct QasSettings {
  std::string output_filename;
  std::vector<std::string> sources;
} QasSettings;

extern QasSettings settings;