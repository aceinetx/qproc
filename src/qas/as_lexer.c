#include <as_lexer.h>
#include <opcodes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

Token token_new() {
	Token tok;
	tok.type = T_NULL;
	tok.value_u = 0;
	tok.value_i = 0;
	tok.line = 0;
	memset(tok.value_s, 0, LEXER_STR_MAX);
	return tok;
}

Lexer *lexer_new(char *code) {
	Lexer *lexer = malloc(sizeof(Lexer));
	lexer->code = code;
	lexer->pos = 0;
	lexer->code_len = strlen(lexer->code);
	lexer->line = 1;

	return lexer;
}

void lexer_delete(Lexer *lexer) {
	free(lexer);
}

bool is_digit(char c) {
	return (c >= '0' && c <= '9');
}

bool is_letter(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '.');
}

bool is_symbol(char c) {
	return (c == '.' || c == ',' || c == '!' || c == ' ');
}

Token lexer_number(Lexer *lexer) {
	Token tok;
	bool is_hex;

	tok = token_new();
	tok.type = T_NUM;
	tok.line = lexer->line;

	is_hex = false;

	while (lexer->pos < lexer->code_len) {
		char c = lexer->code[lexer->pos];
		if (!is_digit(c) && !is_hex) {
			if (c == 'x') {
				is_hex = true;
				tok.value_u = 0;
				lexer->pos++;
				continue;
			} else {
				break;
			}
		}

		if (is_hex) {
			bool invalid;

			invalid = false;
			tok.value_u *= 16;

			if (is_digit(c)) {
				tok.value_u += c - '0';
			} else {
				switch (c) {
				case 'a':
					tok.value_u += 0xa;
					break;
				case 'b':
					tok.value_u += 0xb;
					break;
				case 'c':
					tok.value_u += 0xc;
					break;
				case 'd':
					tok.value_u += 0xd;
					break;
				case 'e':
					tok.value_u += 0xe;
					break;
				case 'f':
					tok.value_u += 0xf;
					break;
				default:
					invalid = true;
					break;
				}
			}
			if (invalid) {
				tok.value_u /= 16;
				break;
			}
		} else {
			tok.value_u *= 10;

			tok.value_u += c - '0';
		}
		lexer->pos++;
	}

	return tok;
}

Token lexer_identifier(Lexer *lexer) {
	Token tok;
	size_t len;

	tok = token_new();
	tok.type = T_IDENTIFIER;
	tok.line = lexer->line;

	len = strlen(tok.value_s);
	while (lexer->pos < lexer->code_len && len < LEXER_STR_MAX) {
		char c = lexer->code[lexer->pos];

		if (!is_letter(c) && !is_digit(c)) {
			if (c == ':') {
				tok.type = T_LABEL;
			}
			break;
		}

		tok.value_s[len] = c;

		len = strlen(tok.value_s);
		lexer->pos++;
	}

	if (tok.value_s[0] == 'r' && len >= 2 && len <= 3) {
		tok.type = T_REGISTER;
	} else if (strcmp(tok.value_s, "sp") == 0) {
		tok.type = T_REGISTER;
	} else if (strcmp(tok.value_s, "bp") == 0) {
		tok.type = T_REGISTER;
	} else if (strcmp(tok.value_s, "ip") == 0) {
		tok.type = T_REGISTER;
	} else if (strcmp(tok.value_s, "dword") == 0) {
		tok.type = T_SIZE;
		tok.value_u = SS_DWORD;
	} else if (strcmp(tok.value_s, "word") == 0) {
		tok.type = T_SIZE;
		tok.value_u = SS_WORD;
	} else if (strcmp(tok.value_s, "byte") == 0) {
		tok.type = T_SIZE;
		tok.value_u = SS_BYTE;
	}

	return tok;
}

Token lexer_directive(Lexer *lexer) {
	Token tok;
	size_t len;

	tok = token_new();
	tok.type = T_DIRECTIVE;
	tok.line = lexer->line;

	len = strlen(tok.value_s);
	while (lexer->pos < lexer->code_len && len < LEXER_STR_MAX) {
		char c = lexer->code[lexer->pos];

		if (!is_letter(c) && !is_digit(c) && c != '#') {
			break;
		}

		tok.value_s[len] = c;

		len = strlen(tok.value_s);
		lexer->pos++;
	}

	return tok;
}

Token lexer_string(Lexer *lexer) {
	Token tok;
	size_t len;
	bool definition;

	tok = token_new();
	tok.type = T_STRING;
	tok.line = lexer->line;

	len = strlen(tok.value_s);
	definition = false;
	while (lexer->pos < lexer->code_len && len < LEXER_STR_MAX) {
		char c = lexer->code[lexer->pos];

		if (!is_letter(c) && !is_digit(c) && !is_symbol(c)) {
			if (c == '"') {
				if (definition) {
					lexer->pos++;
					break;
				}
				definition = true;
				lexer->pos++;
				continue;
			} else {
				break;
			}
		}

		if (definition)
			tok.value_s[len] = c;

		len = strlen(tok.value_s);
		lexer->pos++;
	}

	return tok;
}

Token lexer_next(Lexer *lexer) {
	Token tok;
	bool in_comment = false;
	while (lexer->pos < lexer->code_len) {
		char c = lexer->code[lexer->pos];
		Token token = token_new();

		if (c == ';') {
			in_comment = true;
		}

		if (c == '\n') {
			in_comment = false;
			lexer->line++;
		}

		if (!in_comment) {
			if (is_digit(c)) {
				token = lexer_number(lexer);
			} else if (is_letter(c)) {
				token = lexer_identifier(lexer);
			} else if (c == '#') {
				token = lexer_directive(lexer);
			} else if (c == '"') {
				token = lexer_string(lexer);
			}

			if (token.type != T_NULL)
				return token;
		}

		lexer->pos++;
	}
	tok = token_new();
	tok.type = T_EOF;
	tok.line = lexer->line;
	return tok;
}
