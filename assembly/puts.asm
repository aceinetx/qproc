puts:
	push r0
	push r1
	push r2
	push r3

	; clear r2
	sub r2 r2
	mov r1 1

	; move string address into r3
	mov r3 r0

	putsloop:
		; load current string byte
		lod r0 byte r3
		cmp r0 r2
		be putsend

		swi 0x1

		add r3 r1

	b putsloop
	putsend:
		
	mov r0 10
	swi 0x1

	pop r3
	pop r2	
	pop r1
	pop r0
	pop ip
