#include <cc_compiler.h>
#include <stdlib.h>
#include <string.h>

CCCompiler* cccompiler_new(CCLexer* lexer, FILE* outfd) {
	CCCompiler* compiler = malloc(sizeof(CCCompiler));
	compiler->lexer = lexer;
	compiler->outfd = outfd;

	memset(&compiler->current_function, 0, sizeof(compiler->current_function));
	return compiler;
}

void cccompiler_delete(CCCompiler* compiler) {
	free(compiler);
}

dword get_type_size_from_name(char* name) {
	if (strcmp(name, "void") == 0) {
		return 0;
	} else if (strcmp(name, "i32") == 0) {
		return 4;
	}
	return (dword)-1;
}

bool cccompiler_compile(CCCompiler* this) {
	CCToken token;

	token = cclexer_next(this->lexer);
	while (token.type != CCT_EOF) {
		if (token.type == CCT_IDENTIFIER) {
			dword typesize = get_type_size_from_name(token.value_s);
			if (typesize != (dword)-1) {
				printf("funcdef\n");
				if (!cccompiler_funcdef(this))
					return false;
			}
		}
		token = cclexer_next(this->lexer);
	}

	return true;
}
