; this is the userland code that inits everything as appropriate

[GLOBAL start]
[BITS 32]
[ORG 0x00200000]
start:
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
		;TODO - add something here

		jmp a


