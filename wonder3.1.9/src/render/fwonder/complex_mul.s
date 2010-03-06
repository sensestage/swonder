	.file	"complex_mul.cpp"
	.section	.ctors,"aw",@progbits
	.align 4
	.long	_GLOBAL__I__Z11complex_mulPfS_S_j
	.text
	.align 2
	.p2align 4,,15
.globl _Z11complex_mulPfS_S_j
	.type	_Z11complex_mulPfS_S_j, @function
_Z11complex_mulPfS_S_j:
.LFB1424:
	pushl	%ebp
.LCFI0:
	movl	%esp, %ebp
.LCFI1:
	pushl	%edi
.LCFI2:
	movl	20(%ebp), %eax
	movl	8(%ebp), %edi
	pushl	%esi
.LCFI3:
	movl	16(%ebp), %ecx
	movl	12(%ebp), %esi
	pushl	%ebx
.LCFI4:
	movl	%eax, %ebx
	shrl	$3, %ebx
	je	.L5
	xorl	%eax, %eax
	xorl	%edx, %edx
	.p2align 4,,7
.L4:
	movaps	(%edx,%edi), %xmm2
	movaps	16(%edx,%edi), %xmm3
	movaps	(%edx,%esi), %xmm6
	movaps	16(%edx,%esi), %xmm5
	movaps	%xmm2, %xmm0
	movaps	%xmm3, %xmm1
	mulps	%xmm5, %xmm2
	movaps	16(%edx,%ecx), %xmm4
	mulps	%xmm6, %xmm3
	incl	%eax
	mulps	%xmm6, %xmm0
	mulps	%xmm5, %xmm1
	addps	%xmm3, %xmm2
	subps	%xmm1, %xmm0
	addps	%xmm2, %xmm4
	addps	(%edx,%ecx), %xmm0
	movaps	%xmm4, 16(%edx,%ecx)
	movaps	%xmm0, (%edx,%ecx)
	addl	$32, %edx
	cmpl	%ebx, %eax
	jne	.L4
.L5:
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret
.LFE1424:
	.size	_Z11complex_mulPfS_S_j, .-_Z11complex_mulPfS_S_j
.globl __gxx_personality_v0
	.align 2
	.p2align 4,,15
.globl _Z21complex_mul_overwritePfS_S_j
	.type	_Z21complex_mul_overwritePfS_S_j, @function
_Z21complex_mul_overwritePfS_S_j:
.LFB1425:
	pushl	%ebp
.LCFI5:
	movl	%esp, %ebp
.LCFI6:
	pushl	%edi
.LCFI7:
	movl	20(%ebp), %eax
	movl	8(%ebp), %edi
	pushl	%esi
.LCFI8:
	movl	12(%ebp), %esi
	shrl	$3, %eax
	pushl	%ebx
.LCFI9:
	movl	16(%ebp), %ebx
	je	.L12
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	.p2align 4,,7
.L11:
	movaps	(%edx,%edi), %xmm2
	movaps	16(%edx,%edi), %xmm3
	movaps	%xmm2, %xmm0
	movaps	%xmm3, %xmm1
	movaps	(%edx,%esi), %xmm5
	movaps	16(%edx,%esi), %xmm4
	mulps	%xmm5, %xmm0
	incl	%ecx
	mulps	%xmm4, %xmm1
	mulps	%xmm4, %xmm2
	mulps	%xmm5, %xmm3
	subps	%xmm1, %xmm0
	addps	%xmm3, %xmm2
	movaps	%xmm0, (%edx,%ebx)
	movaps	%xmm2, 16(%edx,%ebx)
	addl	$32, %edx
	cmpl	%eax, %ecx
	jne	.L11
.L12:
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret
.LFE1425:
	.size	_Z21complex_mul_overwritePfS_S_j, .-_Z21complex_mul_overwritePfS_S_j
	.align 2
	.p2align 4,,15
	.type	_Z41__static_initialization_and_destruction_0ii, @function
_Z41__static_initialization_and_destruction_0ii:
.LFB1434:
	pushl	%ebp
.LCFI10:
	movl	%esp, %ebp
.LCFI11:
	subl	$24, %esp
.LCFI12:
	decl	%eax
	je	.L19
.L18:
	leave
	ret
	.p2align 4,,7
.L19:
	cmpl	$65535, %edx
	jne	.L18
	movl	$_ZSt8__ioinit, (%esp)
	call	_ZNSt8ios_base4InitC1Ev
	movl	$__dso_handle, 8(%esp)
	movl	$0, 4(%esp)
	movl	$__tcf_0, (%esp)
	call	__cxa_atexit
	leave
	ret
.LFE1434:
	.size	_Z41__static_initialization_and_destruction_0ii, .-_Z41__static_initialization_and_destruction_0ii
	.align 2
	.p2align 4,,15
	.type	_GLOBAL__I__Z11complex_mulPfS_S_j, @function
_GLOBAL__I__Z11complex_mulPfS_S_j:
.LFB1436:
	pushl	%ebp
.LCFI13:
	movl	$65535, %edx
	movl	$1, %eax
	movl	%esp, %ebp
.LCFI14:
	leave
	jmp	_Z41__static_initialization_and_destruction_0ii
.LFE1436:
	.size	_GLOBAL__I__Z11complex_mulPfS_S_j, .-_GLOBAL__I__Z11complex_mulPfS_S_j
	.align 2
	.p2align 4,,15
	.type	__tcf_0, @function
__tcf_0:
.LFB1435:
	pushl	%ebp
.LCFI15:
	movl	%esp, %ebp
.LCFI16:
	movl	$_ZSt8__ioinit, 8(%ebp)
	leave
	jmp	_ZNSt8ios_base4InitD1Ev
.LFE1435:
	.size	__tcf_0, .-__tcf_0
	.align 2
	.p2align 4,,15
.globl _Z20sse_to_complex_orderPfj
	.type	_Z20sse_to_complex_orderPfj, @function
_Z20sse_to_complex_orderPfj:
.LFB1427:
	pushl	%ebp
.LCFI17:
	movl	%esp, %ebp
.LCFI18:
	movl	12(%ebp), %eax
	shrl	$3, %eax
	je	.L28
	movl	8(%ebp), %edx
	xorl	%ecx, %ecx
	.p2align 4,,7
.L27:
	movaps	(%edx), %xmm0
	movaps	%xmm0, %xmm1
	movaps	16(%edx), %xmm2
	shufps	$238, %xmm2, %xmm0
	movlhps	%xmm2, %xmm1
	incl	%ecx
	shufps	$216, %xmm1, %xmm1
	shufps	$216, %xmm0, %xmm0
	movaps	%xmm1, (%edx)
	movaps	%xmm0, 16(%edx)
	addl	$32, %edx
	cmpl	%ecx, %eax
	jne	.L27
.L28:
	leave
	ret
.LFE1427:
	.size	_Z20sse_to_complex_orderPfj, .-_Z20sse_to_complex_orderPfj
	.align 2
	.p2align 4,,15
.globl _Z20complex_to_sse_orderPfj
	.type	_Z20complex_to_sse_orderPfj, @function
_Z20complex_to_sse_orderPfj:
.LFB1426:
	pushl	%ebp
.LCFI19:
	movl	%esp, %ebp
.LCFI20:
	movl	12(%ebp), %eax
	shrl	$3, %eax
	je	.L34
	movl	8(%ebp), %edx
	xorl	%ecx, %ecx
	.p2align 4,,7
.L33:
	movaps	(%edx), %xmm0
	movaps	%xmm0, %xmm2
	movaps	16(%edx), %xmm1
	shufps	$136, %xmm1, %xmm2
	incl	%ecx
	shufps	$221, %xmm1, %xmm0
	movaps	%xmm2, (%edx)
	movaps	%xmm0, 16(%edx)
	addl	$32, %edx
	cmpl	%ecx, %eax
	jne	.L33
.L34:
	leave
	ret
.LFE1426:
	.size	_Z20complex_to_sse_orderPfj, .-_Z20complex_to_sse_orderPfj
	.local	_ZSt8__ioinit
	.comm	_ZSt8__ioinit,1,1
	.section	.eh_frame,"a",@progbits
.Lframe1:
	.long	.LECIE1-.LSCIE1
.LSCIE1:
	.long	0x0
	.byte	0x1
	.string	"zP"
	.uleb128 0x1
	.sleb128 -4
	.byte	0x8
	.uleb128 0x5
	.byte	0x0
	.long	__gxx_personality_v0
	.byte	0xc
	.uleb128 0x4
	.uleb128 0x4
	.byte	0x88
	.uleb128 0x1
	.align 4
.LECIE1:
.LSFDE5:
	.long	.LEFDE5-.LASFDE5
.LASFDE5:
	.long	.LASFDE5-.Lframe1
	.long	.LFB1434
	.long	.LFE1434-.LFB1434
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI10-.LFB1434
	.byte	0xe
	.uleb128 0x8
	.byte	0x85
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI11-.LCFI10
	.byte	0xd
	.uleb128 0x5
	.align 4
.LEFDE5:
	.ident	"GCC: (GNU) 4.1.1 (Gentoo 4.1.1)"
	.section	.note.GNU-stack,"",@progbits
