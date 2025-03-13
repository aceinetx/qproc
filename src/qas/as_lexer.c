#include <as_lexer.h>
#include <stdbool.h>
#include <stdlib.h>

CALLEOWNS Token *token_new() {
	Token *tok = malloc(sizeof(Token));
	tok->type = T_NUM;
	tok->value_u = 0;
	tok->value_i = 0;
	memset(tok->value_s, 0, LEXER_STR_MAX);
	return tok;
}

Lexer *lexer_new(char *code) {
	Lexer *lexer = malloc(sizeof(Lexer));
	lexer->code = code;
	lexer->pos = 0;

	return lexer;
}

void lexer_delete(Lexer *lexer) {
	free(lexer);
}

bool is_digit(char c) {
	return (c >= '0' && c <= '9');
}

bool is_letter(char c) {
	return (c >= 'a' && c <= 'z' || (c >= 'A' && c <= 'Z') || c == '.');
}

CALLEOWNS Token *lexer_number(Lexer *lexer) {
	Token *tok = token_new();
	tok->type = T_NUM;

	bool is_hex = false;

	while (lexer->pos <= strlen(lexer->code)) {
		char c = lexer->code[lexer->pos];
		if (!is_digit(c) && !is_hex) {
			if (c == 'x') {
				is_hex = true;
				tok->value_u = 0;
				lexer->pos++;
				continue;
			} else {
				break;
			}
		}

		if (is_hex) {
			bool invalid = false;
			tok->value_u *= 16;

			if (is_digit(c)) {
				tok->value_u += c - '0';
			} else {
				switch (c) {
				case 'a':
					tok->value_u += 0xa;
					break;
				case 'b':
					tok->value_u += 0xb;
					break;
				case 'c':
					tok->value_u += 0xc;
					break;
				case 'd':
					tok->value_u += 0xd;
					break;
				case 'e':
					tok->value_u += 0xe;
					break;
				case 'f':
					tok->value_u += 0xf;
					break;
				default:
					invalid = true;
					break;
				}
			}
			if (invalid) {
				tok->value_u /= 16;
				break;
			}
		} else {
			tok->value_u *= 10;

			tok->value_u += c - '0';
		}
		lexer->pos++;
	}

	return tok;
}

CALLEOWNS Token *lexer_identifier(Lexer *lexer) {
	Token *tok = token_new();
	tok->type = T_INSTRUCTION;

	size_t len = strlen(tok->value_s);
	while (lexer->pos <= strlen(lexer->code) && len < LEXER_STR_MAX) {
		char c = lexer->code[lexer->pos];

		if (!is_letter(c) && !is_digit(c)) {
			if (c == ':') {
				tok->type = T_LABEL;
			}
			break;
		}

		tok->value_s[len] = c;

		len = strlen(tok->value_s);
		lexer->pos++;
	}

	if (tok->value_s[0] == 'r' && len >= 2 && len <= 3) {
		tok->type = T_REGISTER;
	} else if (strcmp(tok->value_s, "sp") == 0) {
		tok->type = T_REGISTER;
	} else if (strcmp(tok->value_s, "bp") == 0) {
		tok->type = T_REGISTER;
	} else if (strcmp(tok->value_s, "ip") == 0) {
		tok->type = T_REGISTER;
	}

	return tok;
}

CALLEOWNS Token *lexer_directive(Lexer *lexer) {
	Token *tok = token_new();
	tok->type = T_DIRECTIVE;

	size_t len = strlen(tok->value_s);
	while (lexer->pos <= strlen(lexer->code) && len < LEXER_STR_MAX) {
		char c = lexer->code[lexer->pos];

		if (!is_letter(c) && !is_digit(c) && c != '#') {
			break;
		}

		tok->value_s[len] = c;

		len = strlen(tok->value_s);
		lexer->pos++;
	}

	return tok;
}

CALLEOWNS Token *lexer_string(Lexer *lexer) {
	Token *tok = token_new();
	tok->type = T_STRING;

	size_t len = strlen(tok->value_s);
	bool definition = false;
	while (lexer->pos <= strlen(lexer->code) && len < LEXER_STR_MAX) {
		char c = lexer->code[lexer->pos];

		if (!is_letter(c) && !is_digit(c)) {
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
			tok->value_s[len] = c;

		len = strlen(tok->value_s);
		lexer->pos++;
	}

	return tok;
}

CALLEOWNS Token *lexer_next(Lexer *lexer) {
	while (lexer->pos <= strlen(lexer->code)) {
		char c = lexer->code[lexer->pos];
		if (is_digit(c)) {
			return lexer_number(lexer);
		} else if (is_letter(c)) {
			return lexer_identifier(lexer);
		} else if (c == '#') {
			return lexer_directive(lexer);
		} else if (c == '"') {
			return lexer_string(lexer);
		}
		lexer->pos++;
	}
	Token *tok = token_new();
	tok->type = T_EOF;
	return tok;
}
