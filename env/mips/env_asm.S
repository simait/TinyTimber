/*
 * Copyright (c) 2007, Per Lindgren, Johan Eriksson, Johan Nordlander,
 * Simon Aittamaa.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Luleå University of Technology nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

.global __start
.global mips_context_dispatch

.extern mips_panic
.extern mips_stack_start
.extern mips_timer_interrupt
.extern TMR_CTL

.equ MIPS_CONTEXT_COOKIE,0x55aa55aa

.set noreorder

.section .kdata:
.align 1

interrupt_panic:
	.asciz "Invalid interrupt, not timer compare and no other sources.\n"

interrupt_handler_panic:
	.asciz "Invalid interrupt (NULL).\n"

context_cookie_panic:
	.asciz "Context cookie corrupter.\n"

.section .start,"ax",@progbits
.align 4
__start:
	la		$29, mips_stack_start
	lw		$29, 0($29)
	j		main
	nop

	# NOTE: We should never get here.
	j		mips_panic
	nop

.section .ktext,"ax",@progbits
.align 4

# void mips_interrupt(void);
mips_interrupt:

	subu	$29, 72
.set noat
	sw		$1, 0($sp)
.set at
	sw		$2, 4($sp)
	sw		$3, 8($sp)
	sw		$4, 12($sp)
	sw		$5, 16($sp)
	sw		$6, 20($sp)
	sw		$7, 24($sp)
	sw		$8, 28($sp)
	sw		$9, 32($sp)
	sw		$10, 36($sp)
	sw		$11, 40($sp)
	sw		$12, 44($sp)
	sw		$13, 48($sp)
	sw		$14, 52($sp)
	sw		$15, 56($sp)
	sw		$24, 60($sp)
	sw		$25, 64($sp)
	sw		$31, 68($sp)

	# NOTE:
	#	There might be other interrupts at some point in the
	#	future...

	mfc0	$26, $12
	nop
	andi	$26, $26, 0xfc00
	srl		$26, $26, 0x0a

	# Check the timer interrupt first.
	la		$27, TMR_CTL
	lw		$27, 0($27)
	nop
	andi	$27, $27, 0x10
	beq		$27, $0, .Linterrupt_other
	nop

	la		$27, mips_timer_interrupt
	jalr	$27
	nop

	b		.Linterrupt_done
	nop

.Linterrupt_other:

	andi	$26, $26, 0x3e
	beq		$26, $0, .Linterrupt_panic
	nop
	la		$27, mips_interrupt_handler
	lw		$27, 0($27)
	nop
	beq		$27, $0, .Linterrupt_invalid_handler
	nop
	jalr	$27
	nop
	b		.Linterrupt_done
	nop

.Linterrupt_invalid_handler:

	la		$4, interrupt_handler_panic
	la		$5, mips_panic
	jr		$5
	nop

.Linterrupt_panic:

	la		$4, interrupt_panic
	la		$5, mips_panic
	jr		$5
	nop

.Linterrupt_done:

.set noat
	lw		$1, 0($sp)
.set at
	lw		$2, 4($sp)
	lw		$3, 8($sp)
	lw		$4, 12($sp)
	lw		$5, 16($sp)
	lw		$6, 20($sp)
	lw		$7, 24($sp)
	lw		$8, 28($sp)
	lw		$9, 32($sp)
	lw		$10, 36($sp)
	lw		$11, 40($sp)
	lw		$12, 44($sp)
	lw		$13, 48($sp)
	lw		$14, 52($sp)
	lw		$15, 56($sp)
	lw		$24, 60($sp)
	lw		$25, 64($sp)
	lw		$31, 68($sp)
	addu	$29, 72

	mfc0	$26, $14
	nop
	jr		$26
	rfe

.section .text,"ax",@progbits
.align 4

# void mips_context_dispatch(mips_context_t *);
mips_context_dispatch:
	
	# Save callee-saved registers.
	subu	$29, $29, 48
	sw		$16,  0($29)
	sw		$17,  4($29)
	sw		$18,  8($29)
	sw		$19, 12($29)
	sw		$20, 16($29)
	sw		$21, 20($29)
	sw		$22, 24($29)
	sw		$23, 28($29)
	sw		$28, 32($29)
	sw		$30, 36($29)
	sw		$31, 40($29)

	# Change current to new context.
	la		$2, tt_current
	lw		$3, 0($2)
	sw		$4, 0($2)
	sw		$29, 0($3)
	lw		$5, 4($3)
	nop
	lw		$5, 0($5)
	li		$6, MIPS_CONTEXT_COOKIE
	bne		$5, $6, .Lcontext_cookie_error
	lw		$5, 4($4)
	nop
	lw		$5, 0($5)
	nop
	bne		$5, $6, .Lcontext_cookie_error
	nop
	b		.Lcontext_cookie_ok
	nop

.Lcontext_cookie_error:
	la		$4, context_cookie_panic
	la		$5, mips_panic
	jr		$5
	nop

.Lcontext_cookie_ok:
	lw		$29, 0($4)
	nop

	# Restore callee-saved registers.
	lw		$16,  0($29)
	lw		$17,  4($29)
	lw		$18,  8($29)
	lw		$19, 12($29)
	lw		$20, 16($29)
	lw		$21, 20($29)
	lw		$22, 24($29)
	lw		$23, 28($29)
	lw		$28, 32($29)
	lw		$31, 40($29)
	lw		$30, 36($29)
	jr		$31
	addu	$29, $29, 48

