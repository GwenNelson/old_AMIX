; this is the userland code that inits everything as appropriate
%define sys_debug_out     0x1
%define sys_debug_out_num 0x2
%define sys_get_tid       0x3
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
	; here is where we start doing shit
	; first, test a simple syscall via int 0x80

	mov esi,greeter_string
	call print_string

	mov esi, test_dbg_out_num_string
	call print_string

	push 0xDEADBEEF
	push sys_debug_out_num
	int 0x80
	pop ecx
	pop ecx

	call nl

	mov esi,test_tid_string
	call print_string

	push sys_get_tid
	int 0x80
	pop eax
	
	push eax
	push sys_debug_out_num
	int 0x80
	pop ecx
	pop ecx

	call nl


endless_loop: jmp endless_loop

print_string:
	.run:
	lodsb
	cmp al,0
	je .done
	push eax
	push sys_debug_out
	int 0x80
	pop ecx
	pop ecx
	jmp .run
	.done:
	ret

nl:
	push 10 
	push sys_debug_out
	int 0x80
	pop ecx
	pop ecx
	ret

greeter_string:	         db 'usercode (task 0) running',13,10, 0
test_dbg_out_num_string: db 'Dumping a number, this should say 0xdeadbeef: ',0
test_tid_string:         db 'my TID is ',0