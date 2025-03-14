#pragma once
#include <qvm.h>
#define LEXER_STR_MAX 32

typedef enum {
	T_NULL,
	T_EOF,
	T_NUM,
	T_LABEL,
	T_REGISTER,
	T_IDENTIFIER,
	T_DIRECTIVE,
	T_STRING,
	T_SIZE,
} TokenType;

typedef struct {
	TokenType type;
	char value_s[LEXER_STR_MAX];
	int32_t value_i;
	dword value_u;
	dword line;
} Token;

typedef struct {
	dword pos;
	char *code;
	dword code_len;
	dword line;
} Lexer;

Lexer *lexer_new(char *code);
void lexer_delete(Lexer *lexer);
Token lexer_next(Lexer *lexer);
