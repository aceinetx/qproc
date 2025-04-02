#include <opcodes.h>
#include <qvm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>
#include <vm.h>

int main(int argc, char **argv) {
	char *filename;
	unsigned int i;
	dword size;
	byte *buf;
	bool do_dump_memory;

	if (sizeof(dword) != 4) {
		printf("[qvm] FATAL: sizeof(dword) != 4, cannot continue\n");
		return EXIT_FAILURE;
	}

	filename = NULL;

	for (i = 0; argc; ++i) {
		char *arg;
		(void)i;

		arg = args_shift(&argc, &argv);

		if (strcmp(arg, "--help") == 0) {
			printf("Usage: qvm [OPTION]... [INPUT]\n");
			printf("Options:\n");
			printf("  --help        output this message\n");
			printf("  --dmp         dump memory on qdb instruction hit\n");
			return 0;
		} else if (strcmp(arg, "--dmp") == 0) {
			do_dump_memory = true;
		} else if (i != 0) {
			filename = arg;
		}
	}

	if (!filename) {
		printf("[qvm] no filename provided\n");
		return EXIT_FAILURE;
	}

	buf = fs_read(filename, &size);
	if (buf) {
		VM *vm;

		vm = vm_new();
		vm->do_dump_memory = do_dump_memory;

		memcpy(vm->memory, buf, size);
		free(buf);

		vm_run(vm);

		vm_delete(vm);
	} else {
		printf("[qvm] failed to read a file\n");
		return EXIT_FAILURE;
	}
	return 0;
}
