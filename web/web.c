#include <as_lexer.h>
#include <assembler.h>
#include <stdio.h>

int main() {
	char *src = "mov r0 r1";

	char out[16384];
	memset(out, 0, sizeof(out));
	FILE *fd = fmemopen(out, sizeof(out), "+w");

	Lexer *lexer = lexer_new((char *)src);
	Assembler *assembler = assembler_new(fd, lexer);

	assembler_assemble(assembler);

	assembler_delete(assembler);
	lexer_delete(lexer);

	fclose(fd);

	for (int i = 0; i < 64; i++) {
		printf("%x ", out[i]);
	}
	printf("\n");
}
