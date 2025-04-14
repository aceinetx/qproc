#pragma once
#include <arena.h>
#include <qvm.h>

typedef struct CCLexer {
	char* code;
	dword code_len;
	dword pos;
	dword line;
	Arena strings;
} CCLexer;

typedef enum CCTokenType { CCT_NULL, CCT_EOF, CCT_INTEGER, CCT_STRING, CCT_IDENTIFIER, CCT_LPAREN, CCT_RPAREN, CCT_LSBRACKET, CCT_RSBRACKET, CCT_COMMA, CCT_PLUS, CCT_MINUS, CCT_MUL, CCT_DIV, CCT_EQUAL, CCT_ADDROF, CCT_SEMICOLON, CCT_RETURN } CCTokenType;

typedef struct CCToken {
	CCTokenType type;
	int value_i;
	char* value_s;
	char value_c;
	dword line;
} CCToken;

CCLexer* cclexer_new(char* code);
void cclexer_delete(CCLexer* lexer);

CCToken cclexer_peek(CCLexer* lexer);
CCToken cclexer_next(CCLexer* lexer);
