syscall:
	qdb
	pop ip

setup_syscalls:
	movi r0 0x10200
	movi r1 syscall

	str dword r0 r1

	pop ip
