call main
mov r9 0x1
mov r10 0x2
mov r11 0x3
mov r12 0x4
hlt

scheduler:
	qdb
	mov r0 0x11000
	mov r1 1
	str dword r0 r1
	mov r0 1
	or cr r0
	pop ip

main:
	mov r0 0x11000
	mov r1 1
	str dword r0 r1

	mov r0 0x11004
	mov r1 scheduler
	str dword r0 r1

	mov r0 1
	or cr r0

	pop ip
