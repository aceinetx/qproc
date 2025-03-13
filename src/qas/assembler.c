#include <assembler.h>
#include <stdbool.h>
#include <stdlib.h>

Assembler *assembler_new(FILE *out, Lexer *lexer) {
	Assembler *assembler = malloc(sizeof(Assembler));

	assembler->out = out;
	assembler->lexer = lexer;
	assembler->labels = NULL;

	return assembler;
}

void assembler_add_label(Assembler *this, Label label) {
	if (this->labels == NULL) {
		this->labels = malloc(sizeof(Label) * 2);
		memset(this->labels, 0, sizeof(Label) * 2);
		this->labels[0] = label;
	} else {
		dword len = 0;
		for (int i = 0;; i++) {
			if (this->labels[i].name[0] == '\0') {
				len = i;
				break;
			}
		}
		Label *new_labels = malloc(sizeof(Label) * (len + 1));
		memset(this->labels, 0, sizeof(Label) * (len + 1));
		memcpy(new_labels, this->labels, sizeof(Label) * (len));
		new_labels[len] = label;
		free(this->labels);
		this->labels = new_labels;
	}
}

void assembler_assemble(Assembler *this) {
	bool preprocessor = true;
	for (;;) {
		Token *token = lexer_next(this->lexer);
		if (token->type == T_EOF) {
			if (!preprocessor)
				break;
			preprocessor = false;
			this->lexer->pos = 0;
			continue;
		}

		if (token->type == T_LABEL && preprocessor) {
			Label label;
			memcpy(label.name, token->value_s, LEXER_STR_MAX);
			label.addr = this->addr;
			assembler_add_label(this, label);
		}
	}
}

void assembler_delete(Assembler *this) {
	free(this->labels);
	free(this);
}
