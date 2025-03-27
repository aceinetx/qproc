#pragma once
#include <qvm.h>
#include <vm.h>

void vm_mov(VM *vm, dword *dest, dword *src);
void vm_movi(VM *vm, dword *dest, dword src);
void vm_lod(VM *vm, dword *dest, dword *src, byte size_specifier);
void vm_str(VM *vm, dword *dest_addr, dword *src, byte size_specifier);
void vm_cmp(VM *vm, dword *left, dword *right);
void vm_pushi(VM *vm, dword source);
void vm_push(VM *vm, dword *source);
void vm_pop(VM *vm, dword *dest);
void vm_add(VM *vm, dword *dest, dword *source);
void vm_sub(VM *vm, dword *dest, dword *source);
void vm_call(VM *vm, dword *dest);
void vm_calli(VM *vm, dword dest);
void vm_mul(VM *vm, dword *dest, dword *source);
void vm_div(VM *vm, dword *dest, dword *source);
void vm_swi(VM *vm, byte index);

void vm_be(VM *vm, dword dest);
void vm_bl(VM *vm, dword dest);
void vm_bg(VM *vm, dword dest);
void vm_ble(VM *vm, dword dest);
void vm_bge(VM *vm, dword dest);

void vm_or(VM *vm, dword *dest, dword *source);
void vm_xor(VM *vm, dword *dest, dword *source);
void vm_and(VM *vm, dword *dest, dword *source);
void vm_not(VM *vm, dword *dest);
