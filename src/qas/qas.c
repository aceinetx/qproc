#include <qproc.h>

int main(int argc, char **argv) {
	char *filename, *output_filename;
	dword size;
	byte *buf;
	unsigned int i;

	if (sizeof(dword) != 4) {
		printf("[qvm] FATAL: sizeof(dword) != 4, cannot continue\n");
		return EXIT_FAILURE;
	}

	filename = NULL;
	output_filename = "a.out";

	for (i = 0; argc; ++i) {
		char *arg;
		(void)i;

		arg = args_shift(&argc, &argv);

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

	buf = fs_read(filename, &size);
	if (buf) {
		FILE *out;
		Lexer *lexer;
		Assembler *assembler;

		out = fopen(output_filename, "w");

		lexer = lexer_new((char *)buf);
		assembler = assembler_new(out, lexer);

		assembler_assemble(assembler);

		assembler_delete(assembler);
		lexer_delete(lexer);

		fclose(out);
		free(buf);
	} else {
		printf("[qvm] failed to read a file\n");
		return EXIT_FAILURE;
	}
	return 0;
}
