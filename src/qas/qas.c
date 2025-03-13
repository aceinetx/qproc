#include <as_lexer.h>
#include <assembler.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

int main(int argc, char **argv) {
	if (sizeof(dword) != 4) {
		printf("[qvm] FATAL: sizeof(dword) != 4, cannot continue\n");
		return EXIT_FAILURE;
	}

	char *filename = NULL;
	char *output_filename = "a.out";

	for (unsigned int i = 0; argc; ++i) {
		(void)i;

		char *arg = args_shift(&argc, &argv);

		if (strcmp(arg, "--help") == 0) {
			printf("Usage: qas [OPTION]... [INPUT]\n");
			printf("Options:\n");
			printf("  --help        output this message\n");
			printf("  -o            set output filename\n");
			return 0;
		}
		if (strcmp(arg, "-o") == 0) {
			output_filename = args_shift(&argc, &argv);
		} else if (i != 0) {
			filename = arg;
		}
	}

	if (!filename) {
		printf("[qas] no filename provided\n");
		return EXIT_FAILURE;
	}

	printf("[qas] source: %s\n", filename);
	printf("[qas] output: %s\n", output_filename);

	dword size;
	byte *buf = fs_read(filename, &size);
	if (buf) {
		Lexer *lexer = lexer_new((char *)buf);
		Assembler *assembler = assembler_new(stdout, lexer);

		assembler_assemble(assembler);

		assembler_delete(assembler);
		lexer_delete(lexer);
	} else {
		printf("[qvm] failed to read a file\n");
		return EXIT_FAILURE;
	}
	return 0;
}
