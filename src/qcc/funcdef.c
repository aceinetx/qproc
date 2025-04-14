#include <cc_compiler.h>

bool cccompiler_funcdef(CCCompiler* this) {
	// TODO: implement arguments

	CCToken name = cclexer_next(this->lexer);
	CCToken excepted_lparen = cclexer_next(this->lexer);
	CCToken excepted_rparen = cclexer_next(this->lexer);
	CCToken excepted_lsbracket = cclexer_next(this->lexer);
	if (excepted_lparen.type != CCT_LPAREN) {
		printf("[qcc] [%d] excepted an lparen ('(')\n", name.line);
		return false;
	}
	if (excepted_rparen.type != CCT_RPAREN) {
		printf("[qcc] [%d] excepted an rparen (')')\n", name.line);
		return false;
	}
	if (excepted_lsbracket.type != CCT_LSBRACKET) {
		printf("[qcc] [%d] excepted an lsbracket ('{')\n", name.line);
		return false;
	}

	fprintf(this->outfd, "%s:\n", name.value_s);
	fprintf(this->outfd, "push bp\n");
	fprintf(this->outfd, "mov bp sp\n");

	cccompiler_block(this);

	return true;
}
