#include <cc_lexer.h>
#include <stdlib.h>
#include <string.h>

CCToken cctoken_new(void) {
	CCToken token;
	token.type = CCT_NULL;
	token.value_i = 0;
	token.value_s = NULL;
	token.value_c = 0;
	token.line = 0;
	return token;
}

CCLexer* cclexer_new(char* code) {
	CCLexer* lexer = malloc(sizeof(CCLexer));
	lexer->code = code;
	lexer->code_len = strlen(code);
	lexer->pos = 0;
	lexer->strings = arena_new();
	return lexer;
}

void cclexer_delete(CCLexer* lexer) {
	arena_free(&lexer->strings);
	free(lexer);
}

bool is_digit(char c) {
	return (c >= '0' && c <= '9');
}

bool is_identifier_letter(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

bool is_letter(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '.');
}

bool is_symbol(char c) {
	return (c == '.' || c == ',' || c == '!' || c == ' ');
}

CCToken cclexer_number(CCLexer* lexer) {
	CCToken tok;
	bool is_hex;

	tok = cctoken_new();
	tok.type = CCT_INTEGER;
	tok.line = lexer->line;

	is_hex = false;

	while (lexer->pos < lexer->code_len) {
		char c = lexer->code[lexer->pos];
		if (!is_digit(c) && !is_hex) {
			if (c == 'x') {
				is_hex = true;
				tok.value_i = 0;
				lexer->pos++;
				continue;
			} else {
				break;
			}
		}

		if (is_hex) {
			bool invalid;

			invalid = false;
			tok.value_i *= 16;

			if (is_digit(c)) {
				tok.value_i += c - '0';
			} else {
				switch (c) {
				case 'a':
					tok.value_i += 0xa;
					break;
				case 'b':
					tok.value_i += 0xb;
					break;
				case 'c':
					tok.value_i += 0xc;
					break;
				case 'd':
					tok.value_i += 0xd;
					break;
				case 'e':
					tok.value_i += 0xe;
					break;
				case 'f':
					tok.value_i += 0xf;
					break;
				default:
					invalid = true;
					break;
				}
			}
			if (invalid) {
				tok.value_i /= 16;
				break;
			}
		} else {
			tok.value_i *= 10;

			tok.value_i += c - '0';
		}
		lexer->pos++;
	}

	return tok;
}

CCToken cclexer_identifier(CCLexer* lexer) {
	CCToken tok;
	size_t len;

	tok = cctoken_new();
	tok.type = CCT_IDENTIFIER;
	tok.line = lexer->line;
	tok.value_s = malloc(1);

	len = 1;
	while (lexer->pos < lexer->code_len) {
		char c = lexer->code[lexer->pos];

		if (!is_identifier_letter(c) && !is_digit(c)) {
			break;
		}

		tok.value_s = realloc(tok.value_s, len + 1);
		tok.value_s[len - 1] = c;
		tok.value_s[len] = '\0';
		len++;

		lexer->pos++;
	}

	arena_append(&lexer->strings, tok.value_s);

	return tok;
}

CCToken cclexer_string(CCLexer* lexer) {
	CCToken tok;
	size_t len;
	bool definition;

	tok = cctoken_new();
	tok.type = CCT_STRING;
	tok.line = lexer->line;
	tok.value_s = malloc(1);

	len = 1;

	definition = false;
	while (lexer->pos < lexer->code_len) {
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

		if (definition) {
			tok.value_s = realloc(tok.value_s, len + 1);
			tok.value_s[len - 1] = c;
			tok.value_s[len] = '\0';
			len++;
		}

		lexer->pos++;
	}

	arena_append(&lexer->strings, tok.value_s);

	return tok;
}

CCToken cclexer_next(CCLexer* lexer) {
	CCToken tok;
	bool in_comment = false;
	while (lexer->pos < lexer->code_len) {
		char c = lexer->code[lexer->pos];
		CCToken token = cctoken_new();

		if (c == ';') {
			in_comment = true;
		}

		if (c == '\n') {
			in_comment = false;
			lexer->line++;
		}

		if (!in_comment) {
			if (is_digit(c)) {
				token = cclexer_number(lexer);
			} else if (is_letter(c)) {
				token = cclexer_identifier(lexer);
			} else if (c == '"') {
				token = cclexer_string(lexer);
			} else if (c == '(') {
				token.type = CCT_LPAREN;
				lexer->pos++;
			} else if (c == ')') {
				token.type = CCT_RPAREN;
				lexer->pos++;
			} else if (c == '{') {
				token.type = CCT_LSBRACKET;
				lexer->pos++;
			} else if (c == '}') {
				token.type = CCT_RSBRACKET;
				lexer->pos++;
			}

			if (token.type != CCT_NULL)
				return token;
		}

		lexer->pos++;
	}
	tok = cctoken_new();
	tok.type = CCT_EOF;
	tok.line = lexer->line;
	return tok;
}
