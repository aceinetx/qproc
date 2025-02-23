#include "qvm.h"

#include <stdio.h>

#include <cstring>
#include <string>

#include "bytearray.h"
#include "fs.h"
#include "util.h"
#include "vm.h"

int main(int argc, char** argv) {
	(void) argc;
	(void) argv;

	VM vm;

	std::string filename = "";
	for (unsigned int i = 0; argc; ++i) {
		(void) i;

		std::string arg = args_shift(&argc, &argv);

		if (arg == "--help") {
			printf("Usage: qvm [OPTION]... [INPUT]\n");
			printf("Options:\n");
			printf("  --help        output this message\n");
			printf("  -s            print processor state on certain actions\n");
			printf("    exception   ... on exceptions\n");
			printf("  -i            log instructions\n");
			return 0;
		} else if (arg == "-s") {
			std::string state = args_shift(&argc, &argv);
			if (state == "exception") {
				vm.debug_state = DebugState::EXCEPTION;
			} else {
				fprintf(stderr, "Invalid debug state\n");
				return 1;
			}
		} else if (arg == "-i") {
			vm.log_instructions = true;
		} else if (i != 0) {
			filename = arg;
		}
	}

	if (filename.empty()) {
		fprintf(stdout, "No filename provided\n");
		return 1;
	}

	Result<ByteArray, std::string> result = readFileBytes(filename);

	if (result.is_error()) {
		fprintf(stdout, "Error: no such file or directory: %s (%s)\n",
						filename.c_str(), result.get_error().value().c_str());
		return 1;
	}

	ByteArray code = result.get_success().value();

	memcpy(vm.memory, code.data(), code.size());

	vm.execute();
}
