

_text SEGMENT

ALIGN 8
doswi PROC
	mov r10, rcx
	syscall
	ret
doswi endp

_text ENDS
END