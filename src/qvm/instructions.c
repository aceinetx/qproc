#include <endian.h>
#include <instructions.h>
#include <opcodes.h>
#include <qvm.h>
#include <stdlib.h>
#include <vm.h>

void vm_mov(VM *vm, dword *dest, dword *src) {
	*dest = *src;
}

void vm_movi(VM *vm, dword *dest, dword src) {
	*dest = src;
}

void vm_lod(VM *vm, dword *dest, dword *src_addr, byte size_specifier) {
	switch (size_specifier) {
	case SS_DWORD:
		*dest = fromQendian(&vm->memory[*src_addr]);
		break;
	case SS_WORD:
		*dest = (word)(fromQendian(&vm->memory[*src_addr]));
		break;
	case SS_BYTE:
		*dest = (byte)(fromQendian(&vm->memory[*src_addr]));
		break;
	default:
		break;
	}
}

void vm_str(VM *vm, dword *dest_addr, dword *src, byte size_specifier) {
	byte *src_conv = toQendian(*src);
	switch (size_specifier) {
	case SS_DWORD:
		memcpy(&(vm->memory[*dest_addr]), src_conv, 4);
		break;
	case SS_WORD:
		memcpy(&(vm->memory[*dest_addr]), src_conv, 2);
		break;
	case SS_BYTE:
		memcpy(&(vm->memory[*dest_addr]), src_conv, 1);
		break;
	default:
		break;
	}
	free(src_conv);
}

void vm_cmp(VM *vm, dword *left, dword *right) {
	vm->flags.ZF = 0;
	if (*left == *right)
		vm->flags.ZF = 1;

	vm->flags.CF = 0;
	if (*left < *right)
		vm->flags.CF = 1;
}

void vm_pushi(VM *vm, dword source) {
	byte *src_conv = toQendian(source);
	memcpy(&(vm->memory[vm->regs.sp - 4]), src_conv, 4);
	vm->regs.sp -= 4;
	free(src_conv);
}

void vm_push(VM *vm, dword *source) {
	byte *src_conv = toQendian(*source);
	memcpy(&(vm->memory[vm->regs.sp - 4]), src_conv, 4);
	vm->regs.sp -= 4;
	free(src_conv);
}

void vm_pop(VM *vm, dword *dest) {
	*dest = fromQendian(&vm->memory[vm->regs.sp]);
	vm->regs.sp += 4;
}

void vm_add(VM *vm, dword *dest, dword *source) {
	*dest += *source;
}

void vm_sub(VM *vm, dword *dest, dword *source) {
	*dest -= *source;
}

void vm_mul(VM *vm, dword *dest, dword *source) {
	*dest *= *source;
}

void vm_div(VM *vm, dword *dest, dword *source) {
	*dest /= *source;
}

void vm_swi(VM *vm, byte index) {
	dword swi_table_dest;
	switch (index) {
	case INT_PUTC:
		putc(vm->regs.r0, stdout);
		break;
	case INT_GETC:
		vm->regs.r0 = getchar();
		break;
	case INT_GETS:
		fgets((char *)&vm->memory[vm->regs.r0], vm->regs.r1, stdin);
		break;
	default:
		/* assume it's a call to software interrupt table */
		swi_table_dest = fromQendian(&vm->memory[SWI_TABLE + (index * sizeof(dword))]);

		vm_calli(vm, swi_table_dest);
		break;
	}
}

/* call instructions */

void vm_call(VM *vm, dword *dest) {
	vm_pushi(vm, vm->regs.ip);
	vm->regs.ip = *dest;
}

void vm_calli(VM *vm, dword dest) {
	vm_pushi(vm, vm->regs.ip);
	vm->regs.ip = dest;
}

/* branch instructions */

void vm_be(VM *vm, dword dest) {
	if (vm->flags.ZF == 1) {
		vm->regs.ip = dest;
	}
}

void vm_bl(VM *vm, dword dest) {
	if (vm->flags.CF == 1) {
		vm->regs.ip = dest;
	}
}

void vm_bg(VM *vm, dword dest) {
	if (vm->flags.CF == 0 && vm->flags.ZF == 0) {
		vm->regs.ip = dest;
	}
}

void vm_ble(VM *vm, dword dest) {
	if (vm->flags.CF == 1 || vm->flags.ZF == 1) {
		vm->regs.ip = dest;
	}
}

void vm_bge(VM *vm, dword dest) {
	if (vm->flags.CF == 0 || vm->flags.ZF == 1) {
		vm->regs.ip = dest;
	}
}

/* logical operators */
void vm_or(VM *vm, dword *dest, dword *source) {
	*dest |= *source;
}

void vm_xor(VM *vm, dword *dest, dword *source) {
	*dest ^= *source;
}

void vm_and(VM *vm, dword *dest, dword *source) {
	*dest &= *source;
}
