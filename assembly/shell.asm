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

		mov r12 r0

		mov r1 exit_s
		call strcmp
		sub r1 r1
		cmp r0 r1
		be shell_ret

		mov r0 r12
		call putsn

	b shell_loop

shell_ret:

	mov sp bp
	pop bp

	pop ip
