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

.set PM0, 0x04
.set PRCR, 0x0a

.set ISTACK_SIZE, 0x100

.text
	.global _start
	.global _vector_default

	.extern _main

_start:
	/* Setup processor mode, single chip mode. */
	mov.b	#0x02, PRCR
	mov.b	#0x00, PM0
	mov.b	#0x00, PRCR

	/* Setup the interrupt stack. */
	mov.w	#__ram_end, r0
	ldc		r0, isp
	sub.w	#ISTACK_SIZE, r0

	/* Setup user stack. */
	fset	u
	ldc		r0, sp

	/* Setup interrupt vector. */
	ldc		#%hi16(_vectors_variable), intbh
	ldc		#%lo16(_vectors_variable), intbl

	/* Copy .data. */
	mov.b	#%hi8(__data_start), r1h
	mov.w	#%lo16(__data_start), a0
	mov.w	#__ram_start, a1
	mov.w	#__data_size, r3
	smovf.b

	/* Zero out .bss. */
	mov.b	#0x00, R0L
	mov.w	#__bss_size, r3
	mov.w	#__bss_start, a1
	sstr.b

	/* Enter main(). */
	jmp.a	_main

	/* In case we return, should realy generate a reset :/ */
	jmp.b	0

/* We should probably not get here. */
_vector_default:
	jmp.a	_vector_default

/* Fixed hardware vector table. */
.section .vectors_fixed, "a",@progbits

.size	_vectors_fixed, 36
.type	_vectors_fixed, @object

_vectors_fixed:

.long	0 /* Undefined Instruction. */
.long	0 /* Overflow INTO Instruction. */
.long	0 /* BRK Instruction.*/
.long	0 /* Address Match Interupt. */
.long	0 /* Single Step Interrupt. */
.long	0 /* Watchdog, Oscillation, Voltage Interrupt. */
.long	0 /* DBC. */
.long	0 /* NMI. */
.long	_start /* Reset. */

/* Variable vector table. */
.section .vectors_variable

.size _vectors_variable, 256
.type _vectors_variable, @object

_vectors_variable:

.long	_vector_0
.long	_vector_1
.long	_vector_2
.long	_vector_3
.long	_vector_4
.long	_vector_5
.long	_vector_6
.long	_vector_7
.long	_vector_8
.long	_vector_9
.long	_vector_10
.long	_vector_11
.long	_vector_12
.long	_vector_13
.long	_vector_14
.long	_vector_15
.long	_vector_16
.long	_vector_17
.long	_vector_18
.long	_vector_19
.long	_vector_20
.long	_vector_21
.long	_vector_22
.long	_vector_23
.long	_vector_24
.long	_vector_25
.long	_vector_26
.long	_vector_27
.long	_vector_28
.long	_vector_29
.long	_vector_30
.long	_vector_31
.long	_vector_32
.long	_vector_33
.long	_vector_34
.long	_vector_35
.long	_vector_36
.long	_vector_37
.long	_vector_38
.long	_vector_39
.long	_vector_40
.long	_vector_41
.long	_vector_42
.long	_vector_43
.long	_vector_44
.long	_vector_45
.long	_vector_46
.long	_vector_47
.long	_vector_48
.long	_vector_49
.long	_vector_50
.long	_vector_51
.long	_vector_52
.long	_vector_53
.long	_vector_54
.long	_vector_55
.long	_vector_56
.long	_vector_57
.long	_vector_58
.long	_vector_59
.long	_vector_60
.long	_vector_61
.long	_vector_62
.long	_vector_63
