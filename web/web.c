#include <as_lexer.h>
#include <assembler.h>
#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>

Assembler* assembler;
Lexer* lexer;

EMSCRIPTEN_KEEPALIVE
void all_create() {
	lexer = lexer_new(NULL);
	assembler = assembler_new(NULL, lexer);
	assembler->no_stdout = true;
}

EMSCRIPTEN_KEEPALIVE
void all_destroy() {
	lexer_delete(lexer);
	assembler_delete(assembler);
}

EMSCRIPTEN_KEEPALIVE
char* assemble(char* src) {
	assembler->bytes_assembled = 0;
	assembler->addr = 0;
	lexer->pos = 0;
	lexer->code = src;
	lexer->code_len = strlen(src);

	memset(sources_store, 0, sizeof(sources_store));
	sources_store[0].name = "(js)";
	sources_store[0].start_line = 0;

	assembler_assemble(assembler);

	return assembler->no_fd_buf;
}

EMSCRIPTEN_KEEPALIVE
int get_assembled_bytes() {
	return assembler->bytes_assembled;
}

EMSCRIPTEN_KEEPALIVE
char* get_assembler_logs() {
	return assembler->logs;
}

int main() {
	printf("Initalized\n");
}
