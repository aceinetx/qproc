syscall:
	qdb
	pop ip

setup_syscalls:
	push bp
	mov bp sp

	movi r0 0x10200
	movi r1 syscall

	str dword r0 r1

	mov bp sp
	pop bp
	pop ip
