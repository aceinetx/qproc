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

bool assembler_do_const_operand(Assembler *this, Token token) {
	if (this->preprocessor)
		return true;

	if (token.type == T_NUM) {
		ftoQendian(this->out, token.value_u);
		return true;
	} else if (token.type == T_IDENTIFIER) {
		Label *label = assembler_get_label(this, token.value_s);
		if (!label) {
			printf("[qas] [%d]: undefined label\n", token.line);
			return false;
		}

		ftoQendian(this->out, label->addr);
		return true;
	}

	return false;
}

void assembler_assemble(Assembler *this) {
	this->preprocessor = true;
	for (;;) {
		Token token = lexer_next(this->lexer);
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
			if (strcmp(token.value_s, "mov") == 0) {
				this->addr += 0x2;
				Token left = lexer_next(this->lexer);
				Token right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in mov\n", this->lexer->line);

					break;
				}

				if (right.type != T_REGISTER && right.type != T_NUM && right.type != T_IDENTIFIER) {
					printf("[qas] [%d]: excepted a register || number || label in mov\n", this->lexer->line);

					break;
				} else if (right.type == T_NUM || right.type == T_IDENTIFIER) {
					if (!this->preprocessor) {
						assembler_outb(this, get_register_index_from_name(left.value_s) + MOVI_R0);
						if (!assembler_do_const_operand(this, right)) {

							break;
						}
					}
					this->addr += 0x3;
				} else {
					assembler_outb(this, get_register_index_from_name(left.value_s) + MOV_R0);
					assembler_outb(this, get_register_index_from_name(right.value_s) + MOVI_R0);
				}

			} else if (strcmp(token.value_s, "movi") == 0) {
				this->addr += 0x5;
				Token left = lexer_next(this->lexer);
				Token right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in mov\n", this->lexer->line);

					break;
				}

				if (right.type != T_NUM && right.type != T_IDENTIFIER) {
					printf("[qas] [%d]: excepted a number || label in mov\n", this->lexer->line);

					break;
				}

				assembler_outb(this, get_register_index_from_name(left.value_s) + MOVI_R0);
				if (!assembler_do_const_operand(this, right)) {

					break;
				}

			} else if (strcmp(token.value_s, "hlt") == 0) {
				this->addr += 0x1;
				assembler_outb(this, HLT);
			} else if (strcmp(token.value_s, "nop") == 0) {
				this->addr += 0x1;
				assembler_outb(this, NOP);
			} else if (strcmp(token.value_s, "lod") == 0) {
				Token dest = lexer_next(this->lexer);
				Token size = lexer_next(this->lexer);
				Token src = lexer_next(this->lexer);
				this->addr += 3;

				if (src.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in lod\n", this->lexer->line);

					break;
				}

				if (dest.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in lod\n", this->lexer->line);

					break;
				}

				if (size.type != T_SIZE) {
					printf("[qas] [%d]: excepted a size specifier in lod\n", this->lexer->line);

					break;
				}

				assembler_outb(this, get_register_index_from_name(src.value_s) + LOD_R0);
				assembler_outb(this, size.value_u);
				assembler_outb(this, get_register_index_from_name(dest.value_s));
			} else if (strcmp(token.value_s, "cmp") == 0) {
				Token left = lexer_next(this->lexer);
				Token right = lexer_next(this->lexer);
				this->addr += 0x2;

				if (left.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in cmp\n", this->lexer->line);

					break;
				}

				if (right.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in cmp\n", this->lexer->line);

					break;
				}

				assembler_outb(this, get_register_index_from_name(left.value_s) + CMP_R0);
				assembler_outb(this, get_register_index_from_name(right.value_s));
			} else if (strcmp(token.value_s, "push") == 0) {
				Token op = lexer_next(this->lexer);

				if (op.type != T_REGISTER && op.type != T_NUM) {
					printf("[qas] [%d]: excepted a register || number in push\n", this->lexer->line);

					break;
				} else if (op.type == T_NUM) {
					this->addr += 0x5;
					assembler_outb(this, PUSHI);
					assembler_do_const_operand(this, op);
				} else {
					this->addr += 0x2;
					assembler_outb(this, PUSH);
					assembler_outb(this, get_register_index_from_name(op.value_s));
				}
			} else if (strcmp(token.value_s, "pushi") == 0) {
				Token op = lexer_next(this->lexer);

				if (op.type != T_NUM) {
					printf("[qas] [%d]: excepted a number in pushi\n", this->lexer->line);

					break;
				}
				this->addr += 0x5;
				assembler_outb(this, PUSHI);
				assembler_do_const_operand(this, op);
			} else if (strcmp(token.value_s, "pop") == 0) {
				Token op = lexer_next(this->lexer);

				if (op.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register || number in pop\n", this->lexer->line);

					break;
				}
				this->addr += 0x2;
				assembler_outb(this, POP);
				assembler_outb(this, get_register_index_from_name(op.value_s));
			} else if (token.value_s[0] == 'b') {
				Token op = lexer_next(this->lexer);
				if (op.type != T_REGISTER && op.type != T_NUM && op.type != T_IDENTIFIER) {
					printf("[qas] [%d]: excepted a register || number || label in branch-like instruction\n", this->lexer->line);

					break;
				}

				if (strcmp(token.value_s, "b") == 0) {
					if (op.type == T_REGISTER) {
						assembler_outb(this, MOV_IP);
						assembler_outb(this, get_register_index_from_name(op.value_s));
						this->addr += 0x2;
					} else {
						assembler_outb(this, MOVI_IP);
						assembler_do_const_operand(this, op);
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
						assembler_do_const_operand(this, op);
						this->addr += 0x6;
					} else {
						assembler_outb(this, get_register_index_from_name(op.value_s));
						this->addr += 0x2;
					}
				}
			} else if (strcmp(token.value_s, "str") == 0) {
				Token size = lexer_next(this->lexer);
				Token dest = lexer_next(this->lexer);
				Token src = lexer_next(this->lexer);
				this->addr += 3;

				if (src.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in str\n", this->lexer->line);
					break;
				}

				if (dest.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in str\n", this->lexer->line);
					break;
				}

				if (size.type != T_SIZE) {
					printf("[qas] [%d]: excepted a size specifier in str\n", this->lexer->line);
					break;
				}

				assembler_outb(this, get_register_index_from_name(dest.value_s) + STR_R0);
				assembler_outb(this, size.value_u);
				assembler_outb(this, get_register_index_from_name(src.value_s));
			} else if (strcmp(token.value_s, "add") == 0) {
				this->addr += 0x2;
				Token left = lexer_next(this->lexer);
				Token right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in add\n", this->lexer->line);
					break;
				}

				if (right.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in add\n", this->lexer->line);
					break;
				}
				assembler_outb(this, get_register_index_from_name(left.value_s) + ADD_R0);
				assembler_outb(this, get_register_index_from_name(right.value_s));

			} else if (strcmp(token.value_s, "sub") == 0) {
				this->addr += 0x2;
				Token left = lexer_next(this->lexer);
				Token right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in sub\n", this->lexer->line);
					break;
				}

				if (right.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in sub\n", this->lexer->line);
					break;
				}
				assembler_outb(this, get_register_index_from_name(left.value_s) + SUB_R0);
				assembler_outb(this, get_register_index_from_name(right.value_s));

			} else if (strcmp(token.value_s, "call") == 0) {
				Token op = lexer_next(this->lexer);

				if (op.type != T_REGISTER && op.type != T_IDENTIFIER && op.type != T_NUM) {
					printf("[qas] [%d]: excepted a register || label || number in call\n", this->lexer->line);
					break;
				}

				if (op.type == T_REGISTER) {
					assembler_outb(this, CALL);
					assembler_outb(this, get_register_index_from_name(op.value_s));
					this->addr += 0x2;
				} else {
					assembler_outb(this, CALLI);
					assembler_do_const_operand(this, op);
					this->addr += 0x5;
				}

			} else if (strcmp(token.value_s, "calli") == 0) {
				Token op = lexer_next(this->lexer);

				if (op.type != T_IDENTIFIER && op.type != T_NUM) {
					printf("[qas] [%d]: excepted a label || number in calli\n", this->lexer->line);
					break;
				}

				assembler_outb(this, CALLI);
				assembler_do_const_operand(this, op);
				this->addr += 0x5;

			} else if (strcmp(token.value_s, "qdb") == 0) {
				this->addr += 0x1;
				assembler_outb(this, QDB);
			} else if (strcmp(token.value_s, "mul") == 0) {
				this->addr += 0x2;
				Token left = lexer_next(this->lexer);
				Token right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in mul\n", this->lexer->line);
					break;
				}

				if (right.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in mul\n", this->lexer->line);
					break;
				}
				assembler_outb(this, get_register_index_from_name(left.value_s) + MUL_R0);
				assembler_outb(this, get_register_index_from_name(right.value_s));

			} else if (strcmp(token.value_s, "div") == 0) {
				this->addr += 0x2;
				Token left = lexer_next(this->lexer);
				Token right = lexer_next(this->lexer);

				if (left.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in div\n", this->lexer->line);

					break;
				}

				if (right.type != T_REGISTER) {
					printf("[qas] [%d]: excepted a register in div\n", this->lexer->line);

					break;
				}
				assembler_outb(this, get_register_index_from_name(left.value_s) + DIV_R0);
				assembler_outb(this, get_register_index_from_name(right.value_s));

			} else {
			}
		}
	}
}

void assembler_delete(Assembler *this) {
	free(this->labels);
	free(this);
}
