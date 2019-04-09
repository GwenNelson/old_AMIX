[GLOBAL default_usercode]
[GLOBAL default_usercode_end]
section .rodata
default_usercode:
	incbin "usercode.bin"
default_usercode_end:
