call bootloader_main
hlt

bootloader_main:
	mov r0 msg
	call puts

	call setup_syscalls

	pop ip

msg: #byte "QOS"

