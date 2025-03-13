#include <assembler.h>
#include <endian.h>
#include <opcodes.h>
#include <stdlib.h>

Assembler *assembler_new(FILE *out, Lexer *lexer) {
	Assembler *assembler = malloc(sizeof(Assembler));

	assembler->out = out;
	assembler->lexer = lexer;
	assembler->labels = NULL;

	return assembler;
}

Label *assembler_get_label(Assembler *this, char *name) {
	for (int i = 0;; i++) {
		if (this->labels[i].name[0] == '\0') {
			break;
		}

		if (strcmp(this->labels[i].name, name) == 0) {
			return &(this->labels[i]);
		}
	}
	return NULL;
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

void assembler_outb(Assembler *this, byte b) {
	if (this->preprocessor)
		return;
	fprintf(this->out, "%c", b);
}

byte get_register_index_from_name(char *name) {
	if (strcmp(name, "r0") == 0) {
		return 0;
	} else if (strcmp(name, "r1") == 0) {
		return 1;
	} else if (strcmp(name, "r2") == 0) {
		return 2;
	} else if (strcmp(name, "r3") == 0) {
		return 3;
	} else if (strcmp(name, "r4") == 0) {
		return 4;
	} else if (strcmp(name, "r5") == 0) {
		return 5;
	} else if (strcmp(name, "r6") == 0) {
		return 6;
	} else if (strcmp(name, "r7") == 0) {
		return 7;
	} else if (strcmp(name, "r8") == 0) {
		return 8;
	} else if (strcmp(name, "r9") == 0) {
		return 9;
	} else if (strcmp(name, "r10") == 0) {
		return 10;
	} else if (strcmp(name, "r11") == 0) {
		return 11;
	} else if (strcmp(name, "r12") == 0) {
		return 12;
	} else if (strcmp(name, "sp") == 0) {
		return 13;
	} else if (strcmp(name, "bp") == 0) {
		return 14;
	} else if (strcmp(name, "ip") == 0) {
		return 15;
	}
	return 0;
}

bool assembler_do_const_operand(Assembler *this, Token *token) {
	if (this->preprocessor)
		return true;

	if (token->type == T_NUM) {
		ftoQendian(this->out, token->value_u);
		return true;
	} else if (token->type == T_IDENTIFIER) {
		Label *label = assembler_get_label(this, token->value_s);
		if (!label) {
			printf("[qas] [%d]: undefined label\n", token->line);
			return false;
		}

		ftoQendian(this->out, label->addr);
	}
}

void assembler_assemble(Assembler *this) {
	this->preprocessor = true;
	for (;;) {
		Token *token = lexer_next(this->lexer);
		if (token->type == T_EOF) {
			free(token);
			if (!this->preprocessor)
				break;
			this->preprocessor = false;
			this->lexer->pos = 0;
			this->lexer->line = 1;
			continue;
		}

		if (token->type == T_LABEL && this->preprocessor) {
			Label label;
			memcpy(label.name, token->value_s, LEXER_STR_MAX);
			label.addr = this->addr;
			assembler_add_label(this, label);
		} else if (token->type == T_IDENTIFIER) {
			if (strcmp(token->value_s, "mov") == 0) {
				this->addr += 0x2;
				Token *left = lexer_next(this->lexer);
				Token *right = lexer_next(this->lexer);

				if (left->type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in mov\n", this->lexer->line);
					break;
				}

				if (right->type != T_REGISTER && right->type != T_NUM && right->type != T_IDENTIFIER) {
					printf("[qas] [%d]: excepted a register || number || label in mov\n", this->lexer->line);
					break;
				} else if (right->type == T_NUM || right->type == T_IDENTIFIER) {
					if (!this->preprocessor) {
						assembler_outb(this, get_register_index_from_name(left->value_s) + MOVI_R0);
						if (!assembler_do_const_operand(this, right))
							break;
					}
					this->addr += 0x3;
				} else {
					assembler_outb(this, get_register_index_from_name(left->value_s) + MOV_R0);
					assembler_outb(this, get_register_index_from_name(right->value_s) + MOVI_R0);
				}

				free(left);
				free(right);
			} else if (strcmp(token->value_s, "movi") == 0) {
				this->addr += 0x5;
				Token *left = lexer_next(this->lexer);
				Token *right = lexer_next(this->lexer);

				if (left->type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in mov\n", this->lexer->line);
				}

				if (right->type != T_NUM && right->type != T_IDENTIFIER) {
					printf("[qas] [%d]: excepted a number || label in mov\n", this->lexer->line);
				}

				assembler_outb(this, get_register_index_from_name(left->value_s) + MOVI_R0);
				if (!assembler_do_const_operand(this, right))
					break;

				free(left);
				free(right);
			} else if (strcmp(token->value_s, "hlt") == 0) {
				this->addr += 0x1;
				assembler_outb(this, HLT);
			} else if (strcmp(token->value_s, "nop") == 0) {
				this->addr += 0x1;
				assembler_outb(this, NOP);
			}
		}
		free(token);
	}
}

void assembler_delete(Assembler *this) {
	free(this->labels);
	free(this);
}
