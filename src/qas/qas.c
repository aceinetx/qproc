#include <as_lexer.h>
#include <assembler.h>
#include <qas.h>
#include <qvm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

int main(int argc, char** argv) {
	char *sources[SOURCES_MAX], *output_filename, *buf;
	dword total_size;
	unsigned int i;
	bool glued;

	buf = NULL;

	if (sizeof(dword) != 4) {
		printf("[qas] FATAL: sizeof(dword) != 4, cannot continue\n");
		return EXIT_FAILURE;
	}

	memset(sources, 0, sizeof(sources));
	memset(sources_store, 0, sizeof(sources_store));

	output_filename = "a.out";
	glued = false;

	for (i = 0; argc; ++i) {
		char* arg;
		(void)i;

		arg = args_shift(&argc, &argv);

		if (strcmp(arg, "--help") == 0) {
			printf("Usage: qas [OPTION]... [INPUT]\n");
			printf("Options:\n");
			printf("  --help        output this message\n");
			printf("  -o            set output filename\n");
			printf("  --glued       print glued together sources\n");
			return 0;
		}
		if (strcmp(arg, "-o") == 0) {
			output_filename = args_shift(&argc, &argv);
		} else if (strcmp(arg, "--glued") == 0) {
			glued = true;
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
		dword line;

		line = 0;
		total_size = 0;
		/* Read all inputs */
		for (i = 0; i < sizeof(sources) / sizeof(sources[0]); i++) {
			dword filesize;
			char *f_buf, *c;
			if (sources[i] == NULL)
				continue;

			for (i = 0; i < sizeof(sources_store) / sizeof(sources_store[0]); i++) {
				Source* source = &sources_store[i];
				if (source->name)
					continue;

				source->name = sources[i];
				source->start_line = line;
				break;
			}

			f_buf = (char*)fs_read(sources[i], &filesize);
			c = f_buf;

			if (filesize > 0) {
				while (*c) {
					if (*c == '\n')
						line++;
					c++;
				}

				buf = realloc(buf, total_size + filesize);

				memcpy(buf + total_size, f_buf, filesize);
				total_size += filesize;

				free(f_buf);
			}
		}
	}

	/* Add 10 NULL bytes */
	buf = realloc(buf, total_size + 10);
	memcpy(buf + total_size, "\0\0\0\0\0\0\0\0\0\0", 10);
	total_size += 10;

	if (glued) {
		printf("%s\n", buf);
		free(buf);
		return 0;
	}

	if (buf) {
		FILE* out;
		Lexer* lexer;
		Assembler* assembler;

		out = fopen(output_filename, "w");

		lexer = lexer_new((char*)buf);
		assembler = assembler_new(out, lexer);

		assembler_assemble(assembler);
		printf("[qas] %dB, %fKiB, %fMiB assembled\n", assembler->bytes_assembled, rKiB((float)assembler->bytes_assembled), rMiB((float)assembler->bytes_assembled));

		assembler_delete(assembler);
		lexer_delete(lexer);

		fclose(out);
		free(buf);
	} else {
		printf("[qas] failed to read a file\n");
		return EXIT_FAILURE;
	}
	return 0;
}
