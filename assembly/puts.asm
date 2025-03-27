puts:
	push r0
	push r1
	push r2
	push r3

	sub r2 r2
	mov r3 1

	putsloop:
		lod r1 byte r0

		push r0
		mov r0 r1
		swi 0x1
		pop r0

		add r0 r3

		cmp r1 r2
		bne putsloop

	mov r0 10
	swi 0x1

	pop r3
	pop r2	
	pop r1
	pop r0
	pop ip
