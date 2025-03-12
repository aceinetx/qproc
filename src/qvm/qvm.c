#include <opcodes.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>
#include <vm.h>

int main(int argc, char **argv) {
	if (sizeof(dword) != 4) {
		printf("[qvm] FATAL: sizeof(dword) != 4, cannot continue\n");
		return EXIT_FAILURE;
	}

	char *filename = NULL;

	for (unsigned int i = 0; argc; ++i) {
		(void)i;

		char *arg = args_shift(&argc, &argv);

		if (strcmp(arg, "--help") == 0) {
			printf("Usage: qvm [OPTION]... [INPUT]\n");
			printf("Options:\n");
			printf("  --help        output this message\n");
			return 0;
		} else if (strcmp(arg, "--isa") == 0) {
			printf("%d\n", MOV_R0);
			return 0;
		} else if (i != 0) {
			filename = arg;
		}
	}

	if (!filename) {
		printf("[qvm] no filename provided\n");
		return EXIT_FAILURE;
	}

	dword size;
	byte *buf = fs_read(filename, &size);
	if (buf) {
		VM *vm = vm_new();

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
