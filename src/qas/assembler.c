#include <assembler.h>
#include <endian.h>
#include <opcodes.h>
#include <qas.h>
#include <stdbool.h>
#include <stdlib.h>

const dword LABELS_MAX = 128;

Source sources_store[SOURCES_MAX];

Assembler *assembler_new(FILE *out, Lexer *lexer) {
	Assembler *assembler = malloc(sizeof(Assembler));

	assembler->out = out;
	memset(assembler->logs, 0, sizeof(assembler->logs));
	assembler->no_stdout = false;
	assembler->no_fd_buf_p = assembler->no_fd_buf;
	assembler->lexer = lexer;
	assembler->labels = malloc(sizeof(Label) * LABELS_MAX);
	assembler->bytes_assembled = 0;
	memset(assembler->labels, 0, sizeof(Label) * LABELS_MAX);

	return assembler;
}

Label *assembler_get_label(Assembler *this, char *name) {
	int i;
	for (i = 0;; i++) {
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
	int i;
	for (i = 0;; i++) {
		if (this->labels[i].name[0] == '\0') {
			this->labels[i] = label;
			break;
		}
	}
}

void assembler_outb(Assembler *this, byte b) {
	if (this->preprocessor)
		return;
	this->bytes_assembled++;
	if (this->out != NULL) {
		fprintf(this->out, "%c", b);
	} else {
		if (this->no_fd_buf_p <= this->no_fd_buf + sizeof(this->no_fd_buf)) {
			*this->no_fd_buf_p = b;
			this->no_fd_buf_p++;
		}
	}
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

void assembler_error(Assembler *this, Token *token, const char *msg) {
	char *filename = "(unknown source)";
	dword i, line;

	line = token->line;

	for (i = 0; i < sizeof(sources_store) / sizeof(sources_store[0]); i++) {
		Source *source = &sources_store[i];
		if (!source->name)
			continue;

		if (token->line >= source->start_line) {
			line = token->line - source->start_line;
			filename = source->name;
		}
	}

	snprintf(this->logs, sizeof(this->logs), "[qas] [%s: %d]: %s\n", filename, line, msg);
}

bool assembler_do_const_operand(Assembler *this, Token *token) {
	int i;
	byte *bytes;

	if (this->preprocessor)
		return true;

	if (token->type == T_NUM) {
		bytes = toQendian(token->value_u);
		for (i = 0; i < 4; i++) {
			assembler_outb(this, bytes[i]);
		}
		free(bytes);
		return true;
	} else if (token->type == T_IDENTIFIER) {
		Label *label = assembler_get_label(this, token->value_s);
		if (!label) {
			assembler_error(this, token, "undefined label");
			return false;
		}

		bytes = toQendian(label->addr);
		for (i = 0; i < 4; i++) {
			assembler_outb(this, bytes[i]);
		}
		free(bytes);
		return true;
	}

	return false;
}

void assembler_assemble(Assembler *this) {
	Token token, left, right, op, dest, src, size;

	this->preprocessor = true;
	this->addr = 0;
	for (;;) {
		token = lexer_next(this->lexer);
		if (token.type == T_EOF) {

			if (!this->preprocessor)
				break;
			this->preprocessor = false;
			this->lexer->pos = 0;
			this->lexer->line = 1;
			continue;
		}

		if (token.type == T_LABEL && this->preprocessor) {
			Label label;
			memcpy(label.name, token.value_s, LEXER_STR_MAX);
			label.addr = this->addr;
			assembler_add_label(this, label);
		} else if (token.type == T_IDENTIFIER) {
			/*
			 * INSTRUCTIONS
			 */
			if (strcmp(token.value_s, "mov") == 0) {
				this->addr += 0x2;
				left = lexer_next(this->lexer);
				right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					assembler_error(this, &left, "excepted a register in mov");
					break;
				}

				if (right.type != T_REGISTER && right.type != T_NUM && right.type != T_IDENTIFIER) {
					assembler_error(this, &right, "excepted a register || number || label in mov");
					break;
				} else if (right.type == T_NUM || right.type == T_IDENTIFIER) {
					if (!this->preprocessor) {
						assembler_outb(this, get_register_index_from_name(left.value_s) + MOVI_R0);
						if (!assembler_do_const_operand(this, &right)) {
							break;
						}
					}
					this->addr += 0x3;
				} else {
					assembler_outb(this, get_register_index_from_name(left.value_s) + MOV_R0);
					assembler_outb(this, get_register_index_from_name(right.value_s));
				}

			} else if (strcmp(token.value_s, "movi") == 0) {
				this->addr += 0x5;

				left = lexer_next(this->lexer);
				right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					assembler_error(this, &left, "excepted a register in mov");
					break;
				}

				if (right.type != T_NUM && right.type != T_IDENTIFIER) {
					assembler_error(this, &right, "excepted a number || label in mov");
					break;
				}

				assembler_outb(this, get_register_index_from_name(left.value_s) + MOVI_R0);
				if (!assembler_do_const_operand(this, &right)) {
					break;
				}

			} else if (strcmp(token.value_s, "hlt") == 0) {
				this->addr += 0x1;
				assembler_outb(this, HLT);
			} else if (strcmp(token.value_s, "nop") == 0) {
				this->addr += 0x1;
				assembler_outb(this, NOP);
			} else if (strcmp(token.value_s, "lod") == 0) {
				dest = lexer_next(this->lexer);
				size = lexer_next(this->lexer);
				src = lexer_next(this->lexer);
				this->addr += 3;

				if (src.type != T_REGISTER) {
					assembler_error(this, &src, "excepted a register in lod");
					break;
				}

				if (dest.type != T_REGISTER) {
					assembler_error(this, &dest, "excepted a register in lod");
					break;
				}

				if (size.type != T_SIZE) {
					assembler_error(this, &size, "excepted a size specifier in lod");
					break;
				}

				assembler_outb(this, get_register_index_from_name(src.value_s) + LOD_R0);
				assembler_outb(this, size.value_u);
				assembler_outb(this, get_register_index_from_name(dest.value_s));
			} else if (strcmp(token.value_s, "cmp") == 0) {
				left = lexer_next(this->lexer);
				right = lexer_next(this->lexer);
				this->addr += 0x2;

				if (left.type != T_REGISTER) {
					assembler_error(this, &left, "excepted a register in cmp");
					break;
				}

				if (right.type != T_REGISTER) {
					assembler_error(this, &right, "excepted a register in cmp");
					break;
				}

				assembler_outb(this, get_register_index_from_name(left.value_s) + CMP_R0);
				assembler_outb(this, get_register_index_from_name(right.value_s));
			} else if (strcmp(token.value_s, "push") == 0) {
				op = lexer_next(this->lexer);

				if (op.type != T_REGISTER && op.type != T_NUM) {
					assembler_error(this, &op, "excepted a register || number in push");
					break;
				} else if (op.type == T_NUM) {
					this->addr += 0x5;
					assembler_outb(this, PUSHI);
					assembler_do_const_operand(this, &op);
				} else {
					this->addr += 0x2;
					assembler_outb(this, PUSH);
					assembler_outb(this, get_register_index_from_name(op.value_s));
				}
			} else if (strcmp(token.value_s, "pushi") == 0) {
				op = lexer_next(this->lexer);

				if (op.type != T_NUM) {
					assembler_error(this, &op, "excepted a number in pushi");
					break;
				}
				this->addr += 0x5;
				assembler_outb(this, PUSHI);
				assembler_do_const_operand(this, &op);
			} else if (strcmp(token.value_s, "pop") == 0) {
				op = lexer_next(this->lexer);

				if (op.type != T_REGISTER) {
					assembler_error(this, &op, "excepted a register in pop");
					break;
				}
				this->addr += 0x2;
				assembler_outb(this, POP);
				assembler_outb(this, get_register_index_from_name(op.value_s));
			} else if (token.value_s[0] == 'b') {
				op = lexer_next(this->lexer);
				if (op.type != T_REGISTER && op.type != T_NUM && op.type != T_IDENTIFIER) {
					assembler_error(this, &op, "excepted a register || number || label in branch-like instruction");
					break;
				}

				if (strcmp(token.value_s, "b") == 0) {
					if (op.type == T_REGISTER) {
						assembler_outb(this, MOV_IP);
						assembler_outb(this, get_register_index_from_name(op.value_s));
						this->addr += 0x2;
					} else {
						assembler_outb(this, MOVI_IP);
						assembler_do_const_operand(this, &op);
						this->addr += 0x5;
					}
				} else {
					if (strcmp(token.value_s, "be") == 0) {
						assembler_outb(this, BE);
					} else if (strcmp(token.value_s, "bne") == 0) {
						assembler_outb(this, BNE);
					} else if (strcmp(token.value_s, "bl") == 0) {
						assembler_outb(this, BL);
					} else if (strcmp(token.value_s, "bg") == 0) {
						assembler_outb(this, BG);
					} else if (strcmp(token.value_s, "ble") == 0) {
						assembler_outb(this, BLE);
					} else if (strcmp(token.value_s, "bge") == 0) {
						assembler_outb(this, BGE);
					}

					if (op.type != T_REGISTER) {
						assembler_outb(this, JMP_CONST);
						assembler_do_const_operand(this, &op);
						this->addr += 0x6;
					} else {
						assembler_outb(this, get_register_index_from_name(op.value_s));
						this->addr += 0x2;
					}
				}
			} else if (strcmp(token.value_s, "str") == 0) {
				size = lexer_next(this->lexer);
				dest = lexer_next(this->lexer);
				src = lexer_next(this->lexer);
				this->addr += 3;

				if (src.type != T_REGISTER) {
					assembler_error(this, &src, "excepted a register in str");
					break;
				}

				if (dest.type != T_REGISTER) {
					assembler_error(this, &dest, "excepted a register in str");
					break;
				}

				if (size.type != T_SIZE) {
					assembler_error(this, &size, "excepted a size specifier in str");
					break;
				}

				assembler_outb(this, get_register_index_from_name(dest.value_s) + STR_R0);
				assembler_outb(this, size.value_u);
				assembler_outb(this, get_register_index_from_name(src.value_s));
			} else if (strcmp(token.value_s, "add") == 0) {
				Token left, right;

				this->addr += 0x2;

				left = lexer_next(this->lexer);
				right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					assembler_error(this, &left, "excepted a register in add");
					break;
				}

				if (right.type != T_REGISTER) {
					assembler_error(this, &right, "excepted a register in add");
					break;
				}
				assembler_outb(this, get_register_index_from_name(left.value_s) + ADD_R0);
				assembler_outb(this, get_register_index_from_name(right.value_s));

			} else if (strcmp(token.value_s, "sub") == 0) {
				Token left, right;
				this->addr += 0x2;

				left = lexer_next(this->lexer);
				right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					assembler_error(this, &left, "excepted a register in sub");
					break;
				}

				if (right.type != T_REGISTER) {
					assembler_error(this, &right, "excepted a register in sub");
					break;
				}
				assembler_outb(this, get_register_index_from_name(left.value_s) + SUB_R0);
				assembler_outb(this, get_register_index_from_name(right.value_s));

			} else if (strcmp(token.value_s, "call") == 0) {
				op = lexer_next(this->lexer);

				if (op.type != T_REGISTER && op.type != T_IDENTIFIER && op.type != T_NUM) {
					assembler_error(this, &op, "excepted a register || label || number in call");
					break;
				}

				if (op.type == T_REGISTER) {
					assembler_outb(this, CALL);
					assembler_outb(this, get_register_index_from_name(op.value_s));
					this->addr += 0x2;
				} else {
					assembler_outb(this, CALLI);
					assembler_do_const_operand(this, &op);
					this->addr += 0x5;
				}

			} else if (strcmp(token.value_s, "calli") == 0) {
				op = lexer_next(this->lexer);

				if (op.type != T_IDENTIFIER && op.type != T_NUM) {
					assembler_error(this, &op, "excepted a label || number in call");
					break;
				}

				assembler_outb(this, CALLI);
				assembler_do_const_operand(this, &op);
				this->addr += 0x5;

			} else if (strcmp(token.value_s, "qdb") == 0) {
				this->addr += 0x1;
				assembler_outb(this, QDB);
			} else if (strcmp(token.value_s, "mul") == 0) {
				Token left, right;

				this->addr += 0x2;

				left = lexer_next(this->lexer);
				right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					assembler_error(this, &left, "excepted a register in mul");
					break;
				}

				if (right.type != T_REGISTER) {
					assembler_error(this, &right, "excepted a register in mul");
					break;
				}
				assembler_outb(this, get_register_index_from_name(left.value_s) + MUL_R0);
				assembler_outb(this, get_register_index_from_name(right.value_s));
			} else if (strcmp(token.value_s, "div") == 0) {
				Token left, right;

				this->addr += 0x2;

				left = lexer_next(this->lexer);
				right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					assembler_error(this, &left, "excepted a register in div");
					break;
				}

				if (right.type != T_REGISTER) {
					assembler_error(this, &right, "excepted a register in div");
					break;
				}
				assembler_outb(this, get_register_index_from_name(left.value_s) + DIV_R0);
				assembler_outb(this, get_register_index_from_name(right.value_s));
			} else if (strcmp(token.value_s, "swi") == 0) {
				op = lexer_next(this->lexer);

				if (op.type != T_NUM) {
					assembler_error(this, &op, "excepted a number in swi");
					break;
				}
				this->addr += 0x2;
				assembler_outb(this, SWI);
				assembler_outb(this, op.value_u);
			} else if (strcmp(token.value_s, "or") == 0) {
				Token left, right;

				this->addr += 0x2;

				left = lexer_next(this->lexer);
				right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					assembler_error(this, &left, "excepted a register in or");
					break;
				}

				if (right.type != T_REGISTER) {
					assembler_error(this, &right, "excepted a register in or");
					break;
				}
				assembler_outb(this, get_register_index_from_name(left.value_s) + OR_R0);
				assembler_outb(this, get_register_index_from_name(right.value_s));
			} else if (strcmp(token.value_s, "xor") == 0) {
				Token left, right;

				this->addr += 0x2;

				left = lexer_next(this->lexer);
				right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					assembler_error(this, &left, "excepted a register in xor");
					break;
				}

				if (right.type != T_REGISTER) {
					assembler_error(this, &right, "excepted a register in xor");
					break;
				}
				assembler_outb(this, get_register_index_from_name(left.value_s) + XOR_R0);
				assembler_outb(this, get_register_index_from_name(right.value_s));
			} else if (strcmp(token.value_s, "and") == 0) {
				Token left, right;

				this->addr += 0x2;

				left = lexer_next(this->lexer);
				right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					assembler_error(this, &left, "excepted a register in and");
					break;
				}

				if (right.type != T_REGISTER) {
					assembler_error(this, &right, "excepted a register in and");
					break;
				}
				assembler_outb(this, get_register_index_from_name(left.value_s) + AND_R0);
				assembler_outb(this, get_register_index_from_name(right.value_s));
			} else {
				snprintf(this->logs, sizeof(this->logs), "[qas] [%d]: warn: invalid identifier: %s\n", this->lexer->line, token.value_s);
			}
		} else if (token.type == T_DIRECTIVE) {
			/*
			 * DIRECTIVES
			 */
			if (strcmp(token.value_s, "#byte") == 0) {
				op = lexer_next(this->lexer);
				if (op.type == T_STRING) {
					char *c = op.value_s;
					while (*c != 0) {
						assembler_outb(this, *c);
						this->addr += 0x1;
						c++;
					}
					assembler_outb(this, 0);
					this->addr += 0x1;
				} else if (op.type == T_NUM) {
					assembler_outb(this, op.value_u);
					this->addr += 0x1;
				} else {
					assembler_error(this, &op, "invalid argument type for #byte");
					break;
				}
			} else if (strcmp(token.value_s, "#org") == 0) {
				op = lexer_next(this->lexer);
				if (op.type == T_NUM) {
					this->addr = op.value_u;
				} else {
					assembler_error(this, &op, "invalid argument type for #org");
					break;
				}
			} else {
				assembler_error(this, &token, "invalid directive");
				break;
			}
		}
	}
	if (this->logs[0] != '\0' && !this->no_stdout) {
		printf("%s", this->logs);
	}
}

void assembler_delete(Assembler *this) {
	free(this->labels);
	free(this);
}
