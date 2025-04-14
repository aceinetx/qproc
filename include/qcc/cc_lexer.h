#pragma once
#include <qvm.h>

typedef struct CCLexer {
	char* code;
	dword code_len;
	dword pos;
} CCLexer;

enum CCTokenType { CCT_NULL, CCT_EOF, CCT_INTEGER, CCT_STRING, CCT_IDENTIFIER, CCT_LPAREN, CCT_RPAREN, CCT_LSBRACKET, CCT_RSBRACKET, CCT_COMMA, CCT_STAR, CCT_PLUS, CCT_MINUS, CCT_MUL, CCT_DIV, CCT_SETTO };
