#include <cc_compiler.h>

bool cccompiler_block(CCCompiler* this) {
	CCToken token;

	token = cclexer_next(this->lexer);
	while (token.type != CCT_RSBRACKET) {
		if (token.type == CCT_RETURN) {
			if (cclexer_peek(this->lexer).type == CCT_SEMICOLON) {
				fprintf(this->outfd, "mov bp sp\n");
				fprintf(this->outfd, "pop bp\n");
				fprintf(this->outfd, "pop ip\n");
			}
		}
		token = cclexer_next(this->lexer);
	}

	return true;
}
