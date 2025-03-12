#include <instructions.h>
#include <opcodes.h>

void vm_mov(VM *vm, dword *dest, dword *src) {
	*dest = *src;
}

void vm_movc(VM *vm, dword *dest, dword src) {
	*dest = src;
}

void vm_lod(VM *vm, dword *dest, dword *src_addr, byte size_specifier) {
	switch (size_specifier) {
	case SS_DWORD:
		*dest = vm->memory[*src_addr];
		break;
	case SS_WORD:
		*dest = (word)(vm->memory[*src_addr]);
		break;
	case SS_BYTE:
		*dest = (byte)(vm->memory[*src_addr]);
		break;
	default:
		break;
	}
}

void vm_str(VM *vm, dword *dest_addr, dword *src, byte size_specifier) {
	switch (size_specifier) {
	case SS_DWORD:
		memcpy(&(vm->memory[*dest_addr]), src, 4);
		break;
	case SS_WORD:
		memcpy(&(vm->memory[*dest_addr]), src, 2);
		break;
	case SS_BYTE:
		memcpy(&(vm->memory[*dest_addr]), src, 1);
		break;
	default:
		break;
	}
}

void vm_cmp(VM *vm, dword *left, dword *right) {
	vm->flags.ZF = 0;
	if (*left == *right)
		vm->flags.ZF = 1;

	vm->flags.CF = 0;
	if (*left < *right)
		vm->flags.CF = 1;
}

void vm_pushc(VM *vm, dword source) {
	memcpy(&(vm->memory[vm->regs.sp - 4]), &source, 4);
	vm->regs.sp -= 4;
}

void vm_push(VM *vm, dword *source) {
	memcpy(&(vm->memory[vm->regs.sp - 4]), source, 4);
	vm->regs.sp -= 4;
}

void vm_pop(VM *vm, dword *dest) {
	*dest = vm->memory[vm->regs.sp];
	vm->regs.sp += 4;
}

void vm_add(VM *vm, dword *dest, dword *source) {
	*dest += *source;
}

void vm_sub(VM *vm, dword *dest, dword *source) {
	*dest -= *source;
}

void vm_call(VM *vm, dword *dest) {
	vm_pushc(vm, vm->regs.ip);
	vm->regs.ip = *dest;
}

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
