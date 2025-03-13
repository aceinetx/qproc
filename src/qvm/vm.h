#pragma once
#include <qvm.h>
#include <registers.h>
#include <stdio.h>
#define DISASM_STR_SIZE 255

typedef struct {
	byte *memory;
	char *last_disassembly;
	registers regs;
	struct {
		byte CF;
		byte ZF;
	} flags;
} VM;

VM *vm_new(void);
void vm_delete(VM *vm);

dword *vm_get_register_from_index(VM *vm, byte index);
void vm_get_forward(VM *vm, byte **buf, byte n);
void vm_do_instruction(VM *vm);
void vm_run(VM *vm);

void vm_fprint_state(VM *vm, FILE *fd);
