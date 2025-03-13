#pragma once
#include <qvm.h>
#define LEXER_STR_MAX 32

typedef enum {
	T_EOF,
	T_NUM,
	T_LABEL,
	T_REGISTER,
	T_INSTRUCTION,
	T_DIRECTIVE,
	T_STRING,
} TokenType;

typedef struct {
	TokenType type;
	char value_s[LEXER_STR_MAX];
	int32_t value_i;
	uint32_t value_u;
} Token;

typedef struct {
	dword pos;
	char *code;
} Lexer;

Lexer *lexer_new(char *code);
void lexer_delete(Lexer *lexer);
CALLEOWNS Token *lexer_next(Lexer *lexer);
