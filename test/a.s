mov r0 0
mov r1 0x45
str dword r0 r1
lod r3 dword r0

.crash
pop r0
jmp .crash
