call bootloader_main
hlt

bootloader_main:
	push bp
	mov bp sp

	mov r0 msg
	call putsn

	call setup_syscalls

	call shell

	mov bp sp
	pop bp

	pop ip

msg: #byte "QOS"

