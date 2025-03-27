#include <instructions.h>
#include <opcodes.h>
#include <stdlib.h>
#include <vm.h>

VM *vm_new(void) {
	VM *vm = malloc(sizeof(VM));
	vm->memory = malloc(MEMORY_SIZE);
	memset(vm->memory, 0, MEMORY_SIZE);
	memset(&vm->regs, 0, sizeof(registers));
	memset(&vm->flags, 0, sizeof(vm->flags));

	vm->last_disassembly = malloc(DISASM_STR_SIZE);
	memset(vm->last_disassembly, 0, DISASM_STR_SIZE);

	vm->regs.sp = MEMORY_SIZE;
	vm->regs.bp = vm->regs.sp;
	vm->regs.ip = 0;

	printf("[qvm] VM init\n");

	return vm;
}

void vm_delete(VM *vm) {
	free(vm->memory);
	free(vm->last_disassembly);
	free(vm);

	printf("[qvm] VM delete\n");
}

dword *vm_get_register_from_index(VM *vm, byte index) {
	switch (index) {
	case 0:
		return &vm->regs.r0;
	case 1:
		return &vm->regs.r1;
	case 2:
		return &vm->regs.r2;
	case 3:
		return &vm->regs.r3;
	case 4:
		return &vm->regs.r4;
	case 5:
		return &vm->regs.r5;
	case 6:
		return &vm->regs.r6;
	case 7:
		return &vm->regs.r7;
	case 8:
		return &vm->regs.r8;
	case 9:
		return &vm->regs.r9;
	case 10:
		return &vm->regs.r10;
	case 11:
		return &vm->regs.r11;
	case 12:
		return &vm->regs.r12;
	case 13:
		return &vm->regs.sp;
	case 14:
		return &vm->regs.bp;
	case 15:
		return &vm->regs.ip;
	default:
		return &vm->regs.r0;
	}
	return &vm->regs.r0;
}

CALLEOWNS char *vm_get_regname_from_index(VM *vm, byte index) {
	char *buf = malloc(4);
	switch (index) {
	case 0:
		strncpy(buf, "r0", 3);
		break;
	case 1:
		strncpy(buf, "r1", 3);
		break;
	case 2:
		strncpy(buf, "r2", 3);
		break;
	case 3:
		strncpy(buf, "r3", 3);
		break;
	case 4:
		strncpy(buf, "r4", 3);
		break;
	case 5:
		strncpy(buf, "r5", 3);
		break;
	case 6:
		strncpy(buf, "r6", 3);
		break;
	case 7:
		strncpy(buf, "r7", 3);
		break;
	case 8:
		strncpy(buf, "r8", 3);
		break;
	case 9:
		strncpy(buf, "r9", 3);
		break;
	case 10:
		strncpy(buf, "r10", 4);
		break;
	case 11:
		strncpy(buf, "r11", 4);
		break;
	case 12:
		strncpy(buf, "r12", 4);
		break;
	case 13:
		strncpy(buf, "sp", 3);
		break;
	case 14:
		strncpy(buf, "bp", 3);
		break;
	case 15:
		strncpy(buf, "ip", 3);
		break;
	default:
		strncpy(buf, "???", 4);
		break;
	}
	return buf;
}

void vm_get_forward(VM *vm, byte **buf, byte n) {
	dword start;

	*buf = malloc(n);
	start = vm->regs.ip;

	while (vm->regs.ip < start + n) {
		(*buf)[vm->regs.ip - start] = vm->memory[vm->regs.ip];
		vm->regs.ip++;
	}
}

void vm_do_instruction(VM *vm) {
	byte *bytes;

	byte first_byte = vm->memory[vm->regs.ip];
	if (first_byte >= MOV_R0 && first_byte <= MOV_IP) {
		char *a, *b;

		vm_get_forward(vm, &bytes, 2);

		vm_mov(vm, vm_get_register_from_index(vm, first_byte - MOV_R0), vm_get_register_from_index(vm, bytes[1]));

		a = vm_get_regname_from_index(vm, first_byte);
		b = vm_get_regname_from_index(vm, bytes[1]);
		snprintf(vm->last_disassembly, DISASM_STR_SIZE, "mov %s %s", a, b);
		free(a);
		free(b);

		free(bytes);
	} else if (first_byte >= MOVI_R0 && first_byte <= MOVI_IP) {
		char *a;
		dword b;

		vm_get_forward(vm, &bytes, 5);

		vm_movi(vm, vm_get_register_from_index(vm, first_byte - MOVI_R0), fromQendian(&bytes[1]));

		a = vm_get_regname_from_index(vm, first_byte);
		b = fromQendian(&bytes[1]);

		snprintf(vm->last_disassembly, DISASM_STR_SIZE, "mov %s %d  @ movi %s %d", a, b, a, b);
		free(a);

		free(bytes);
	} else if (first_byte == HLT) {
		vm->regs.ip = 0xffffffff;
		printf("[qvm] halted\n");
	} else if (first_byte == NOP) {
		vm->regs.r0 = vm->regs.r0;
		vm->regs.ip++;
	} else if (first_byte >= LOD_R0 && first_byte <= LOD_IP) {
		vm_get_forward(vm, &bytes, 3);

		vm_lod(vm, vm_get_register_from_index(vm, bytes[2]), vm_get_register_from_index(vm, first_byte - LOD_R0), bytes[1]);
		free(bytes);
	} else if (first_byte >= CMP_R0 && first_byte <= CMP_IP) {
		vm_get_forward(vm, &bytes, 2);

		vm_cmp(vm, vm_get_register_from_index(vm, first_byte - CMP_R0), vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else if (first_byte == PUSH) {
		vm_get_forward(vm, &bytes, 2);

		vm_push(vm, vm_get_register_from_index(vm, bytes[1]));

		free(bytes);
	} else if (first_byte == PUSHI) {
		vm_get_forward(vm, &bytes, 5);

		vm_pushi(vm, fromQendian(&(bytes[1])));

		free(bytes);
	} else if (first_byte == POP) {
		vm_get_forward(vm, &bytes, 2);

		vm_pop(vm, vm_get_register_from_index(vm, bytes[1]));

		free(bytes);
	} else if (first_byte >= BE && first_byte <= BGE) {
		dword dest;

		vm_get_forward(vm, &bytes, 6);
		vm->regs.ip -= 6;

		if (bytes[1] == JMP_CONST) {
			dest = fromQendian(&(bytes[2]));
			vm->regs.ip += 6;
		} else {
			dest = *vm_get_register_from_index(vm, bytes[1]);
			vm->regs.ip += 2;
		}

		switch (bytes[0]) {
		case BE:
			vm_be(vm, dest);
			break;
		case BNE:
			vm->flags.ZF = !vm->flags.ZF;
			vm_be(vm, dest);
			break;
		case BL:
			vm_bl(vm, dest);
			break;
		case BG:
			vm_bg(vm, dest);
			break;
		case BLE:
			vm_ble(vm, dest);
			break;
		case BGE:
			vm_bge(vm, dest);
			break;
		}

		free(bytes);
	} else if (first_byte >= STR_R0 && first_byte <= STR_IP) {
		vm_get_forward(vm, &bytes, 3);

		vm_str(vm, vm_get_register_from_index(vm, bytes[0] - STR_R0), vm_get_register_from_index(vm, bytes[2]), bytes[1]);
		free(bytes);
	} else if (first_byte >= ADD_R0 && first_byte <= ADD_IP) {
		vm_get_forward(vm, &bytes, 2);

		vm_add(vm, vm_get_register_from_index(vm, bytes[0] - ADD_R0), vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else if (first_byte >= SUB_R0 && first_byte <= SUB_IP) {
		vm_get_forward(vm, &bytes, 2);

		vm_sub(vm, vm_get_register_from_index(vm, bytes[0] - SUB_R0), vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else if (first_byte == CALL) {
		vm_get_forward(vm, &bytes, 2);

		vm_call(vm, vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else if (first_byte == CALLI) {
		vm_get_forward(vm, &bytes, 5);

		vm_calli(vm, fromQendian(&bytes[1]));
		free(bytes);
	} else if (first_byte == QDB) {
		vm_fprint_state(vm, stdout);
		vm->regs.ip++;
	} else if (first_byte >= MUL_R0 && first_byte <= MUL_IP) {
		vm_get_forward(vm, &bytes, 2);

		vm_mul(vm, vm_get_register_from_index(vm, bytes[0] - MUL_R0), vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else if (first_byte >= DIV_R0 && first_byte <= DIV_IP) {
		vm_get_forward(vm, &bytes, 2);

		vm_div(vm, vm_get_register_from_index(vm, bytes[0] - DIV_R0), vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else if (first_byte == SWI) {
		vm_get_forward(vm, &bytes, 2);
		vm_swi(vm, bytes[1]);
		free(bytes);
	} else if (first_byte >= OR_R0 && first_byte <= OR_IP) {
		vm_get_forward(vm, &bytes, 2);

		vm_or(vm, vm_get_register_from_index(vm, bytes[0] - OR_R0), vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else if (first_byte >= XOR_R0 && first_byte <= XOR_IP) {
		vm_get_forward(vm, &bytes, 2);

		vm_xor(vm, vm_get_register_from_index(vm, bytes[0] - XOR_R0), vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else if (first_byte >= AND_R0 && first_byte <= AND_IP) {
		vm_get_forward(vm, &bytes, 2);

		vm_and(vm, vm_get_register_from_index(vm, bytes[0] - AND_R0), vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else {
		printf("[qvm] illegal instruction\n");
		vm->regs.ip = 0xffffffff;
	}
}

void vm_run(VM *vm) {
	while (vm->regs.ip < MEMORY_SIZE) {
		vm_do_instruction(vm);
	}
}

void vm_fprint_state(VM *vm, FILE *fd) {
	fprintf(fd, "[qvm] VM state:\n");
	fprintf(fd, "[qvm] R0: 0x%x", vm->regs.r0);
	fprintf(fd, " R1: 0x%x", vm->regs.r1);
	fprintf(fd, " R2: 0x%x", vm->regs.r2);
	fprintf(fd, " R3: 0x%x", vm->regs.r3);
	fprintf(fd, " R4: 0x%x", vm->regs.r4);
	fprintf(fd, "\n[qvm] R5: 0x%x", vm->regs.r5);
	fprintf(fd, " R6: 0x%x", vm->regs.r6);
	fprintf(fd, " R7: 0x%x", vm->regs.r7);
	fprintf(fd, " R8: 0x%x", vm->regs.r8);
	fprintf(fd, " R9: 0x%x", vm->regs.r9);
	fprintf(fd, "\n[qvm] R10: 0x%x", vm->regs.r10);
	fprintf(fd, " R11: 0x%x", vm->regs.r11);
	fprintf(fd, " R12: 0x%x", vm->regs.r12);
	fprintf(fd, "\n[qvm] SP: 0x%x", vm->regs.sp);
	fprintf(fd, " BP: 0x%x", vm->regs.bp);
	fprintf(fd, " IP: 0x%x", vm->regs.ip);
	fprintf(fd, "\n[qvm] ZF: 0x%x CF: 0x%x\n", vm->flags.ZF, vm->flags.CF);
}
