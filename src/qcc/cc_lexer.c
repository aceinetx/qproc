#include <cc_lexer.h>
#include <stdlib.h>

CCLexer* cclexer_new(char* code) {
	CCLexer* lexer = malloc(sizeof(CCLexer));
	lexer->code = code;
	lexer->code_len = sizeof(code);
	lexer->pos = 0;
	return lexer;
}

void cclexer_delete(CCLexer* lexer) {
	free(lexer);
}
