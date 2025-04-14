#include <cc_compiler.h>
#include <cc_lexer.h>
#include <qcc.h>
#include <qvm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

Source sources_store[SOURCES_MAX];

int main(int argc, char** argv) {
	char *sources[SOURCES_MAX], *output_filename, *buf;
	dword total_size;
	unsigned int i;
	bool glued;

	buf = NULL;

	if (sizeof(dword) != 4) {
		printf("[qcc] FATAL: sizeof(dword) != 4, cannot continue\n");
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
			printf("Usage: qcc [OPTION]... [INPUT]\n");
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
		printf("[qcc] no sources provided\n");
		return EXIT_FAILURE;
	}

	printf("[qcc] sources: ");
	for (i = 0; i < sizeof(sources) / sizeof(sources[0]); i++) {
		if (sources[i] == NULL)
			continue;
		printf("%s ", sources[i]);
	}
	printf("\n[qcc] output: %s\n", output_filename);

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

			if (filesize != (dword)-1) {
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
		CCLexer* lexer;
		CCCompiler* compiler;

		out = fopen(output_filename, "w");

		lexer = cclexer_new(buf);
		compiler = cccompiler_new(lexer, out);

		cccompiler_compile(compiler);

		cccompiler_delete(compiler);
		cclexer_delete(lexer);

		fclose(out);
		free(buf);
	} else {
		printf("[qcc] failed to read a file\n");
		return EXIT_FAILURE;
	}
	return 0;
}
