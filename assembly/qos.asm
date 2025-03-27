call bootloader_main
hlt

bootloader_main:
	mov r0 msg
	call puts

	call setup_syscalls

	swi 0x80

	pop ip

msg: #byte "QOS"

