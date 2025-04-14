#pragma once
#include <cc_lexer.h>
#include <stdio.h>

typedef struct FunctionMetadata {
	dword ret_size;
	char name[64];
} FunctionMetadata;

typedef struct CCCompiler {
	CCLexer* lexer;
	FILE* outfd;

	FunctionMetadata current_function;
} CCCompiler;

CCCompiler* cccompiler_new(CCLexer* lexer, FILE* outfd);
void cccompiler_delete(CCCompiler* compiler);

bool cccompiler_funcdef(CCCompiler* compiler);
bool cccompiler_block(CCCompiler* compiler);
bool cccompiler_expr(CCCompiler* compiler);

bool cccompiler_compile(CCCompiler* compiler);
