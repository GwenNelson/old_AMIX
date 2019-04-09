; this is the userland code that inits everything as appropriate



[GLOBAL start]
[BITS 32]
[ORG 0x00200000]
[MAP all usercode.map]


start:
     mov esp,0x00202000
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
	mov esi, greeter_string
	call print_string

	; here is where we start doing shit
	; first, test a simple syscall via int 0x80

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
	pop ebx



	push ebx
	push sys_debug_out_num
	int 0x80
	pop ecx
	pop ecx

	call nl


	and ebx,4095

	mov eax, 0xC03FF000
	add eax, ebx
endless_loop:
	add ebx,1
	mov dword [eax], ebx

 	jmp endless_loop

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

greeter_string:          db 10,'[USERCODE]',9, 'Hello from default usercode',10,0
test_dbg_out_num_string: db    '[USERCODE]',9, 'Dumping a number, this should say 0xdeadbeef: ',0
test_tid_string:         db    '[USERCODE]',9, 'My TID is ',0

%define X(syscall_num,syscall_name) sys_ %+ syscall_name equ syscall_num
	%[%include "kernel/syscalls.def"]
%undef X


