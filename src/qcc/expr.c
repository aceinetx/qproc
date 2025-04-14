#include <cc_compiler.h>

bool cccompiler_expr(CCCompiler* this) {
	CCToken token;

	token = cclexer_next(this->lexer);
	while (token.type != CCT_RPAREN && token.type != CCT_SEMICOLON) {
		token = cclexer_next(this->lexer);
	}

	return true;
}
