; this is the userland code that inits everything as appropriate

[GLOBAL start]
[GLOBAL enter_user]
[BITS 32]
[ORG 0x00200000]
start:

;enter_user:
;		mov ax, 0x23	; user mode data selector is 0x20 (GDT entry 3). Also sets RPL to 3
;		mov ds, ax
;		mov es, ax
;		mov fs, ax
;		mov gs, ax
;
;		push 0x23		; SS, notice it uses same selector as above
;		push esp		; ESP
;		pushfd			; EFLAGS
;		push 0x1b		; CS, user mode code selector is 0x18. With RPL 3 this is 0x1b
;		lea eax, [a]		; EIP first
;		push eax
;
;		sti
;		iretd
     sti
     mov ax,0x23
     mov ds,ax
     mov es,ax 
     mov fs,ax 
     mov gs,ax ;we don't need to worry about SS. it's handled by iret
 
     mov eax,esp
     push 0x23 ;user data segment with bottom 2 bits set for ring 3
     push eax ;push our current stack just for the heck of it
     pushf
     push 0x1B; ;user code segment with bottom 2 bits set for ring 3
     push a ;may need to remove the _ for this to work right 
     iret

	a:
		mov eax,0
		int 0x80

;		hlt
		jmp a
