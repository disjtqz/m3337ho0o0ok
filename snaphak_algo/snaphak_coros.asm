_text SEGMENT


PUBLIC coroswitch

sto_gp_coro  macro TO, N, REGVAL
	mov [TO+(N*8)+160], REGVAL
endm

ld_gp_coro macro FROM, N, REGTO
	mov REGTO,[FROM+(N*8)+160]
endm

coroswitch PROC
	
	;rcx = coro we're switching from
	;rdx = coro we're switching to
	pop r8
	;r8 contains the return address for our current coro, we need to save it
	;so when our state is restored we execute at the point we switched from
	
	;RBX, RBP, RDI, RSI, RSP, R12, R13, R14, R15, and XMM6-15

	movaps [rcx], xmm6
	movaps [rcx+16], xmm7
	movaps [rcx+32], xmm8
	movaps [rcx+48], xmm9
	movaps [rcx+64], xmm10
	movaps [rcx+80], xmm11
	movaps [rcx+96], xmm12
	movaps [rcx+112], xmm13
	movaps [rcx+128], xmm14
	movaps [rcx+144], xmm15

	sto_gp_coro rcx, 0, rbx
	sto_gp_coro rcx, 1, rbp

	sto_gp_coro rcx, 2, rdi
	sto_gp_coro rcx, 3, rsi
	sto_gp_coro rcx, 4, rsp
	sto_gp_coro rcx, 5, r12
	sto_gp_coro rcx, 6, r13
	sto_gp_coro rcx, 7, r14
	sto_gp_coro rcx, 8, r15
	sto_gp_coro rcx, 9, r8

	movaps xmm6, [rdx]
	movaps xmm7, [rdx+16]
	movaps xmm8, [rdx+32]
	mov r8, rcx
	movaps xmm9, [rdx+48]
	movaps xmm10, [rdx+64]

	movaps xmm11, [rdx+80]
	movaps xmm12, [rdx+96]
	movaps xmm13, [rdx+112]

	movaps xmm14, [rdx+128]
	movaps xmm15, [rdx+144]

	
	ld_gp_coro rdx, 0, rbx
	ld_gp_coro rdx, 1, rbp

	ld_gp_coro rdx, 2, rdi
	ld_gp_coro rdx, 3, rsi
	ld_gp_coro rdx, 4, rsp
	ld_gp_coro rdx, 5, r12
	ld_gp_coro rdx, 6, r13
	ld_gp_coro rdx, 7, r14
	ld_gp_coro rdx, 8, r15
	ld_gp_coro rdx, 9, r9
	push r9
	ld_gp_coro rdx, 10, rax
	ld_gp_coro rdx, 11, rcx
	ret


coroswitch endp

_text ENDS

END