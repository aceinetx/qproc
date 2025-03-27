shell:
	push bp
	mov bp sp
	
	mov r0 shell_msg
	call putsn

	shell_loop:
		mov r0 arrow_s
		call puts

		mov r1 64
		mov r0 bp
		sub r0 r1
		swi 0x3

		call puts

	b shell_loop

	mov sp bp
	pop bp

	pop ip

shell_msg: #byte "QOS Shell"
arrow_s: #byte 62 #byte 0x20 #byte 0
