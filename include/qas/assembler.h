#pragma once
#include <as_lexer.h>
#include <qvm.h>
#include <stdbool.h>
#include <stdio.h>

extern const dword LABELS_MAX;

typedef struct {
	char name[LEXER_STR_MAX];
	dword addr;
} Label;

typedef struct {
	FILE *out;
	Lexer *lexer;
	dword addr;
	Label *labels;
	bool preprocessor;
	dword bytes_assembled;
	char no_fd_buf[32767]; /* A buffer for cases if FILE* out is NULL, we assume that's intended and instead output the bytes into this buffer */
	char *no_fd_buf_p;
	char logs[512];
	bool no_stdout;
} Assembler;

Assembler *assembler_new(FILE *out, Lexer *lexer);
void assembler_delete(Assembler *assembler);

Label *assembler_get_label(Assembler *assembler, char *name);
void assembler_add_label(Assembler *assembler, Label label);
void assembler_assemble(Assembler *assembler);
