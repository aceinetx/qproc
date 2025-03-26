#include <as_lexer.h>
#include <assembler.h>
#include <qvm.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

int main(int argc, char **argv) {
	char *sources[64], *output_filename, *buf;
	dword total_size;
	unsigned int i;

	buf = NULL;

	if (sizeof(dword) != 4) {
		printf("[qvm] FATAL: sizeof(dword) != 4, cannot continue\n");
		return EXIT_FAILURE;
	}

	memset(sources, 0, sizeof(sources));
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
			size_t i;

			for (i = 0; i < sizeof(sources) / sizeof(sources[0]); i++) {
				if (sources[i] != NULL)
					continue;
				sources[i] = arg;
				break;
			}
		}
	}

	if (!sources[0]) {
		printf("[qas] no sources provided\n");
		return EXIT_FAILURE;
	}

	printf("[qas] sources: ");
	for (i = 0; i < sizeof(sources) / sizeof(sources[0]); i++) {
		if (sources[i] == NULL)
			continue;
		printf("%s ", sources[i]);
	}
	printf("\n[qas] output: %s\n", output_filename);

	{
		total_size = 0;
		/* Read all inputs */
		for (i = 0; i < sizeof(sources) / sizeof(sources[0]); i++) {
			dword filesize;
			char *f_buf;
			if (sources[i] == NULL)
				continue;

			f_buf = (char *)fs_read(sources[i], &filesize);
			if (filesize > 0) {
				buf = realloc(buf, total_size + filesize);

				memcpy(buf + total_size, f_buf, filesize);
				total_size += filesize;

				free(f_buf);
			}
		}
	}

	/* Add 5 newlines at the end */
	buf = realloc(buf, total_size + 5);
	memcpy(buf + total_size, "\n\n\n\n\n", 5);
	total_size += 5;
	/* And 5 NULL bytes */
	buf = realloc(buf, total_size + 5);
	memcpy(buf + total_size, "\0\0\0\0\0", 5);
	total_size += 5;

	printf("%s\n", buf);

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
