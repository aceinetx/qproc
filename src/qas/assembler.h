#pragma once
#include <as_lexer.h>
#include <qvm.h>
#include <stdio.h>

typedef struct {
	char name[LEXER_STR_MAX];
	dword addr;
} Label;

typedef struct {
	FILE *out;
	Lexer *lexer;
	dword addr;
	Label *labels;
} Assembler;

Assembler *assembler_new(FILE *out, Lexer *lexer);
void assembler_delete(Assembler *assembler);

void assembler_add_label(Assembler *assembler, Label label);
void assembler_assemble(Assembler *assembler);
