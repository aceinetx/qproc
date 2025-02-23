#include "qas.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <format>
#include <fstream>
#include <string>

#include "assembler.h"
#include "fs.h"
#include "opcodes.h"
#include "util.h"

QasSettings settings;

int main(int argc, char** argv) {
	settings.output_filename = "a.out";
	settings.sources = {};

	printf("QVM assembler\n");

	for (unsigned int i = 0; argc; ++i) {
		std::string arg = args_shift(&argc, &argv);
		if (arg == "-o") {
			settings.output_filename = args_shift(&argc, &argv);
		} else if (arg == "--help") {
			printf("Usage: qas [OPTION]... [INPUT]\n");
			printf("Options:\n");
			printf("  -o            set output filename\n");
			printf("  --help        output this message\n");
			return 0;
		} else if (i != 0) {
			settings.sources.push_back(arg);
		}
	}

	printf("Output filename: %s\n", settings.output_filename.c_str());
	printf("Sources: ");
	for (std::string& filename : settings.sources) {
		printf("%s ", filename.c_str());
	}
	printf("\n");

	if (settings.sources.empty()) {
		fprintf(stderr, "error: No sources provided\n");
		return 1;
	}

	Assembler assembler;

	for (std::string& filename : settings.sources) {
		Result<std::string, std::string> result = readFile(filename);
		if (result.is_error()) {
			printf("error: no such file or directory: %s (%s)\n", filename.c_str(),
						 result.get_error().value().c_str());
			return 1;
		}

		assembler.code += std::format(";; QAS: start of {}\n", filename) +
											result.get_success().value() + "\n";
	}

	Result<ByteArray, std::string> assemble_result = assembler.assemble();

	if (assemble_result.is_error()) {
		printf("Error: %s\n", assemble_result.get_error().value().c_str());
		printf("Assembling failed\n");
		return 1;
	}

	ByteArray output = assemble_result.get_success().value();

	std::ofstream output_file(settings.output_filename,
														std::ios::binary | std::ios::out);
	output_file.write((const char*) output.data(), output.size());
	output_file.close();

	printf("Assembled successfully\n");
}