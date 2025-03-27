shell:
	push bp
	mov bp sp
	
	mov r0 shell_msg
	call putsn

	shell_loop:
		sub r11 r11

		mov r0 arrow_s
		call puts

		mov r1 64
		mov r0 bp
		sub r0 r1
		swi 0x3

		mov r12 r0

		; exit
		mov r0 r12
		mov r1 exit_s
		call strcmp
		cmp r0 r11
		be shell_ret

		; restart
		mov r0 r12
		mov r1 restart_s
		call strcmp
		cmp r0 r11
		be shell_restart

		; qdb
		mov r0 r12
		mov r1 qdb_s
		call strcmp
		cmp r0 r11
		be shell_qdb

		mov r0 r12
		call putsn

	b shell_loop

shell_qdb:
	qdb
	b shell_loop

shell_restart:
	call restart

shell_ret:

	mov sp bp
	pop bp

	pop ip
