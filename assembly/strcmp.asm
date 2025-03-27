strcmp:
	; reconstructed from glibc

	push bp
	mov bp sp

	; r0 - p1 (s1), r1 - p2 (s2)

	push r1
	push r2
	push r3
	push r4
	push r5

	mov r4 1
	sub r5 r5

	strcmp_loop:
		lod r2 byte r0
		lod r3 byte r1

		add r0 r4
		add r1 r4

		cmp r2 r5
		be strcmp_ret

	cmp r2 r3
	be strcmp_loop	
		
strcmp_ret:
	sub r2 r3
	mov r0 r2

	pop r5
	pop r4
	pop r3
	pop r2
	pop r1

	mov sp bp
	pop bp

	pop ip
