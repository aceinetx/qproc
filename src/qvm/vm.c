#include <endian.h>
#include <instructions.h>
#include <opcodes.h>
#include <stdio.h>
#include <stdlib.h>
#include <vm.h>

VM *vm_new(void) {
	VM *vm = malloc(sizeof(VM));
	vm->memory = malloc(MEMORY_SIZE);
	memset(vm->memory, 0, MEMORY_SIZE);

	vm->regs.sp = MEMORY_SIZE;
	vm->regs.bp = vm->regs.sp;
	vm->regs.ip = 0;

	printf("[qvm] VM init\n");

	return vm;
}

void vm_delete(VM *vm) {
	free(vm->memory);
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

void vm_get_forward(VM *vm, byte **buf, byte n) {
	*buf = malloc(n);
	dword start = vm->regs.ip;
	while (vm->regs.ip < start + n) {
		(*buf)[vm->regs.ip - start] = vm->memory[vm->regs.ip];
		vm->regs.ip++;
	}
}

void vm_do_instruction(VM *vm) {
	byte *bytes;

	byte first_byte = vm->memory[vm->regs.ip];
	if (first_byte >= MOV_R0 && first_byte <= MOV_IP) {
		vm_get_forward(vm, &bytes, 2);

		vm_mov(vm, vm_get_register_from_index(vm, first_byte - MOV_R0),
					 vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else if (first_byte >= MOVC_R0 && first_byte <= MOVC_IP) {
		vm_get_forward(vm, &bytes, 5);

		vm_movc(vm, vm_get_register_from_index(vm, first_byte - MOVC_R0),
						fromQendian(&bytes[1]));
		free(bytes);
	} else if (first_byte == HLT) {
		vm->regs.ip = 0xffffffff;
		printf("[qvm] halted\n");
	} else if (first_byte == NOP) {
		vm->regs.r0 = vm->regs.r0;
		vm->regs.ip++;
	} else if (first_byte >= LOD_R0 && first_byte <= LOD_IP) {
		vm_get_forward(vm, &bytes, 3);

		vm_lod(vm, vm_get_register_from_index(vm, bytes[2]),
					 vm_get_register_from_index(vm, first_byte - LOD_R0), bytes[1]);
		free(bytes);
	} else if (first_byte >= CMP_R0 && first_byte <= CMP_IP) {
		vm_get_forward(vm, &bytes, 2);

		vm_cmp(vm, vm_get_register_from_index(vm, first_byte - CMP_R0),
					 vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else if (first_byte == PUSH) {
		vm_get_forward(vm, &bytes, 2);

		vm_push(vm, vm_get_register_from_index(vm, bytes[1]));

		free(bytes);
	} else if (first_byte == PUSHC) {
		vm_get_forward(vm, &bytes, 5);

		vm_pushc(vm, fromQendian(&(bytes[1])));

		free(bytes);
	} else if (first_byte == POP) {
		vm_get_forward(vm, &bytes, 2);

		vm_pop(vm, vm_get_register_from_index(vm, bytes[1]));

		free(bytes);
	} else if (first_byte >= BE && first_byte <= BGE) {
		vm_get_forward(vm, &bytes, 6);
		vm->regs.ip -= 6;

		dword dest;
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

		vm_str(vm, vm_get_register_from_index(vm, bytes[0] - STR_R0),
					 vm_get_register_from_index(vm, bytes[2]), bytes[1]);
		free(bytes);
	} else if (first_byte >= ADD_R0 && first_byte <= ADD_IP) {
		vm_get_forward(vm, &bytes, 2);

		vm_add(vm, vm_get_register_from_index(vm, bytes[0] - ADD_R0),
					 vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else if (first_byte >= SUB_R0 && first_byte <= SUB_IP) {
		vm_get_forward(vm, &bytes, 2);

		vm_sub(vm, vm_get_register_from_index(vm, bytes[0] - SUB_R0),
					 vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else if (first_byte == CALL) {
		vm_get_forward(vm, &bytes, 2);

		vm_call(vm, vm_get_register_from_index(vm, bytes[1]));
		free(bytes);
	} else {
		printf("[qvm] illegal instruction\n");
	}
}

void vm_run(VM *vm) {
	while (vm->regs.ip < MEMORY_SIZE) {
		vm_do_instruction(vm);
	}
}

void vm_fprint_state(VM *vm, FILE *fd) {
	fprintf(fd, "[qvm] VM state:");
	fprintf(fd, "[qvm] R0: %d");
}
