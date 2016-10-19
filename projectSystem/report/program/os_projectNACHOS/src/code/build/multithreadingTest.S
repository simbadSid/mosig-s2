	.section .mdebug.abi32
	.previous
	.nan	legacy
	.gnu_attribute 4, 1
	.text
$Ltext0:
	.cfi_sections	.debug_frame
	.align	2
	.globl	strlen
$LFB0 = .
	.file 1 "../test/multithreadingTest.c"
	.loc 1 19 0
	.cfi_startproc
	.set	nomips16
	.set	nomicromips
	.ent	strlen
	.type	strlen, @function
strlen:
	.frame	$fp,16,$31		# vars= 8, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-16
	.cfi_def_cfa_offset 16
	sw	$fp,12($sp)
	.cfi_offset 30, -4
	move	$fp,$sp
	.cfi_def_cfa_register 30
	sw	$4,16($fp)
	.loc 1 20 0
	sw	$0,0($fp)
	.loc 1 21 0
	lw	$2,16($fp)
	nop
	sw	$2,4($fp)
	.loc 1 23 0
	j	$L2
	nop

$L3:
	.loc 1 25 0
	lw	$2,4($fp)
	nop
	addiu	$2,$2,1
	sw	$2,4($fp)
	.loc 1 26 0
	lw	$2,0($fp)
	nop
	addiu	$2,$2,1
	sw	$2,0($fp)
$L2:
	.loc 1 23 0
	lw	$2,4($fp)
	nop
	lb	$2,0($2)
	nop
	bne	$2,$0,$L3
	nop

	.loc 1 28 0
	lw	$2,0($fp)
	.loc 1 29 0
	move	$sp,$fp
	.cfi_def_cfa_register 29
	lw	$fp,12($sp)
	addiu	$sp,$sp,16
	.cfi_restore 30
	.cfi_def_cfa_offset 0
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	strlen
	.cfi_endproc
$LFE0:
	.size	strlen, .-strlen
	.align	2
	.globl	threadFunction1
$LFB1 = .
	.loc 1 35 0
	.cfi_startproc
	.set	nomips16
	.set	nomicromips
	.ent	threadFunction1
	.type	threadFunction1, @function
threadFunction1:
	.frame	$fp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	.cfi_def_cfa_offset 24
	sw	$31,20($sp)
	sw	$fp,16($sp)
	.cfi_offset 31, -4
	.cfi_offset 30, -8
	move	$fp,$sp
	.cfi_def_cfa_register 30
	sw	$4,24($fp)
	.loc 1 36 0
	li	$4,10			# 0xa
	jal	PutChar
	nop

	.loc 1 37 0
	li	$4,9			# 0x9
	jal	PutChar
	nop

	.loc 1 38 0
	li	$4,49			# 0x31
	jal	PutChar
	nop

	.loc 1 39 0
	li	$4,45			# 0x2d
	jal	PutChar
	nop

	.loc 1 40 0
	li	$4,97			# 0x61
	jal	PutChar
	nop

	.loc 1 41 0
	li	$4,97			# 0x61
	jal	PutChar
	nop

	.loc 1 42 0
	li	$4,97			# 0x61
	jal	PutChar
	nop

	.loc 1 43 0
	li	$4,98			# 0x62
	jal	PutChar
	nop

	.loc 1 44 0
	li	$4,10			# 0xa
	jal	PutChar
	nop

	.loc 1 45 0
	jal	UserThreadExit
	nop

	.loc 1 46 0
	move	$sp,$fp
	.cfi_def_cfa_register 29
	lw	$31,20($sp)
	lw	$fp,16($sp)
	addiu	$sp,$sp,24
	.cfi_restore 30
	.cfi_restore 31
	.cfi_def_cfa_offset 0
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	threadFunction1
	.cfi_endproc
$LFE1:
	.size	threadFunction1, .-threadFunction1
	.align	2
	.globl	threadFunction2
$LFB2 = .
	.loc 1 48 0
	.cfi_startproc
	.set	nomips16
	.set	nomicromips
	.ent	threadFunction2
	.type	threadFunction2, @function
threadFunction2:
	.frame	$fp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	.cfi_def_cfa_offset 24
	sw	$31,20($sp)
	sw	$fp,16($sp)
	.cfi_offset 31, -4
	.cfi_offset 30, -8
	move	$fp,$sp
	.cfi_def_cfa_register 30
	sw	$4,24($fp)
	.loc 1 49 0
	li	$4,9			# 0x9
	jal	PutChar
	nop

	.loc 1 50 0
	li	$4,50			# 0x32
	jal	PutChar
	nop

	.loc 1 51 0
	li	$4,45			# 0x2d
	jal	PutChar
	nop

	.loc 1 52 0
	li	$4,122			# 0x7a
	jal	PutChar
	nop

	.loc 1 53 0
	li	$4,122			# 0x7a
	jal	PutChar
	nop

	.loc 1 54 0
	li	$4,122			# 0x7a
	jal	PutChar
	nop

	.loc 1 55 0
	li	$4,121			# 0x79
	jal	PutChar
	nop

	.loc 1 56 0
	li	$4,10			# 0xa
	jal	PutChar
	nop

	.loc 1 57 0
	move	$sp,$fp
	.cfi_def_cfa_register 29
	lw	$31,20($sp)
	lw	$fp,16($sp)
	addiu	$sp,$sp,24
	.cfi_restore 30
	.cfi_restore 31
	.cfi_def_cfa_offset 0
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	threadFunction2
	.cfi_endproc
$LFE2:
	.size	threadFunction2, .-threadFunction2
	.align	2
	.globl	threadFunction3
$LFB3 = .
	.loc 1 59 0
	.cfi_startproc
	.set	nomips16
	.set	nomicromips
	.ent	threadFunction3
	.type	threadFunction3, @function
threadFunction3:
	.frame	$fp,32,$31		# vars= 8, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	.cfi_def_cfa_offset 32
	sw	$31,28($sp)
	sw	$fp,24($sp)
	.cfi_offset 31, -4
	.cfi_offset 30, -8
	move	$fp,$sp
	.cfi_def_cfa_register 30
	sw	$4,32($fp)
	.loc 1 60 0
	lw	$2,32($fp)
	nop
	lw	$2,0($2)
	nop
	sw	$2,16($fp)
	.loc 1 62 0
	li	$4,9			# 0x9
	jal	PutChar
	nop

	.loc 1 63 0
	lw	$4,16($fp)
	jal	PutInt
	nop

	.loc 1 64 0
	li	$4,99			# 0x63
	jal	PutChar
	nop

	.loc 1 65 0
	li	$4,10			# 0xa
	jal	PutChar
	nop

	.loc 1 66 0
	move	$sp,$fp
	.cfi_def_cfa_register 29
	lw	$31,28($sp)
	lw	$fp,24($sp)
	addiu	$sp,$sp,32
	.cfi_restore 30
	.cfi_restore 31
	.cfi_def_cfa_offset 0
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	threadFunction3
	.cfi_endproc
$LFE3:
	.size	threadFunction3, .-threadFunction3
	.align	2
	.globl	threadFunction4
$LFB4 = .
	.loc 1 68 0
	.cfi_startproc
	.set	nomips16
	.set	nomicromips
	.ent	threadFunction4
	.type	threadFunction4, @function
threadFunction4:
	.frame	$fp,32,$31		# vars= 8, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	.cfi_def_cfa_offset 32
	sw	$31,28($sp)
	sw	$fp,24($sp)
	.cfi_offset 31, -4
	.cfi_offset 30, -8
	move	$fp,$sp
	.cfi_def_cfa_register 30
	sw	$4,32($fp)
	.loc 1 69 0
	lw	$2,32($fp)
	nop
	lw	$2,0($2)
	nop
	sw	$2,16($fp)
	.loc 1 70 0
	lw	$2,32($fp)
	nop
	lw	$2,4($2)
	nop
	sw	$2,20($fp)
	.loc 1 72 0
	li	$4,9			# 0x9
	jal	PutChar
	nop

	.loc 1 73 0
	lw	$4,16($fp)
	jal	PutInt
	nop

	.loc 1 74 0
	li	$4,32			# 0x20
	jal	PutChar
	nop

	.loc 1 75 0
	lw	$2,20($fp)
	nop
	sll	$2,$2,24
	sra	$2,$2,24
	move	$4,$2
	jal	PutChar
	nop

	.loc 1 76 0
	li	$4,10			# 0xa
	jal	PutChar
	nop

	.loc 1 77 0
	move	$sp,$fp
	.cfi_def_cfa_register 29
	lw	$31,28($sp)
	lw	$fp,24($sp)
	addiu	$sp,$sp,32
	.cfi_restore 30
	.cfi_restore 31
	.cfi_def_cfa_offset 0
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	threadFunction4
	.cfi_endproc
$LFE4:
	.size	threadFunction4, .-threadFunction4
	.align	2
	.globl	threadFunctionJoin
$LFB5 = .
	.loc 1 79 0
	.cfi_startproc
	.set	nomips16
	.set	nomicromips
	.ent	threadFunctionJoin
	.type	threadFunctionJoin, @function
threadFunctionJoin:
	.frame	$fp,32,$31		# vars= 8, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	.cfi_def_cfa_offset 32
	sw	$31,28($sp)
	sw	$fp,24($sp)
	.cfi_offset 31, -4
	.cfi_offset 30, -8
	move	$fp,$sp
	.cfi_def_cfa_register 30
	sw	$4,32($fp)
	.loc 1 80 0
	lw	$2,32($fp)
	nop
	lw	$2,0($2)
	nop
	sw	$2,16($fp)
	.loc 1 81 0
	lw	$4,16($fp)
	jal	UserThreadJoin
	nop

	.loc 1 82 0
	move	$sp,$fp
	.cfi_def_cfa_register 29
	lw	$31,28($sp)
	lw	$fp,24($sp)
	addiu	$sp,$sp,32
	.cfi_restore 30
	.cfi_restore 31
	.cfi_def_cfa_offset 0
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	threadFunctionJoin
	.cfi_endproc
$LFE5:
	.size	threadFunctionJoin, .-threadFunctionJoin
	.rdata
	.align	2
$LC0:
	.ascii	"\012****Failed to create thread tid1***\012\000"
	.align	2
$LC1:
	.ascii	"\012****Failed to create thread tid2***\012\000"
	.align	2
$LC2:
	.ascii	"\012****Failed to create thread tid3***\012\000"
	.text
	.align	2
	.globl	testCreation
$LFB6 = .
	.loc 1 88 0
	.cfi_startproc
	.set	nomips16
	.set	nomicromips
	.ent	testCreation
	.type	testCreation, @function
testCreation:
	.frame	$fp,40,$31		# vars= 16, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40
	.cfi_def_cfa_offset 40
	sw	$31,36($sp)
	sw	$fp,32($sp)
	.cfi_offset 31, -4
	.cfi_offset 30, -8
	move	$fp,$sp
	.cfi_def_cfa_register 30
	.loc 1 89 0
	li	$2,3			# 0x3
	sw	$2,28($fp)
	.loc 1 92 0
	lui	$2,%hi(threadFunction1)
	addiu	$4,$2,%lo(threadFunction1)
	move	$5,$0
	jal	UserThreadCreate
	nop

	sw	$2,16($fp)
	.loc 1 93 0
	lw	$2,16($fp)
	nop
	bgtz	$2,$L11
	nop

	.loc 1 95 0
	lui	$2,%hi($LC0)
	addiu	$4,$2,%lo($LC0)
	li	$5,100			# 0x64
	jal	PutString
	nop

	.loc 1 96 0
	j	$L10
	nop

$L11:
	.loc 1 98 0
	lui	$2,%hi(threadFunction2)
	addiu	$4,$2,%lo(threadFunction2)
	move	$5,$0
	jal	UserThreadCreate
	nop

	sw	$2,20($fp)
	.loc 1 99 0
	lw	$2,20($fp)
	nop
	bgtz	$2,$L13
	nop

	.loc 1 101 0
	lui	$2,%hi($LC1)
	addiu	$4,$2,%lo($LC1)
	li	$5,100			# 0x64
	jal	PutString
	nop

	.loc 1 102 0
	j	$L10
	nop

$L13:
	.loc 1 104 0
	addiu	$3,$fp,28
	lui	$2,%hi(threadFunction3)
	addiu	$4,$2,%lo(threadFunction3)
	move	$5,$3
	jal	UserThreadCreate
	nop

	sw	$2,24($fp)
	.loc 1 105 0
	lw	$2,24($fp)
	nop
	bgtz	$2,$L14
	nop

	.loc 1 107 0
	lui	$2,%hi($LC2)
	addiu	$4,$2,%lo($LC2)
	li	$5,100			# 0x64
	jal	PutString
	nop

	.loc 1 108 0
	j	$L10
	nop

$L14:
	.loc 1 117 0
	lw	$4,16($fp)
	jal	UserThreadJoin
	nop

	.loc 1 118 0
	lw	$4,20($fp)
	jal	UserThreadJoin
	nop

	.loc 1 119 0
	lw	$4,24($fp)
	jal	UserThreadJoin
	nop

$L10:
	.loc 1 121 0
	move	$sp,$fp
	.cfi_def_cfa_register 29
	lw	$31,36($sp)
	lw	$fp,32($sp)
	addiu	$sp,$sp,40
	.cfi_restore 30
	.cfi_restore 31
	.cfi_def_cfa_offset 0
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	testCreation
	.cfi_endproc
$LFE6:
	.size	testCreation, .-testCreation
	.align	2
	.globl	testJoin
$LFB7 = .
	.loc 1 123 0
	.cfi_startproc
	.set	nomips16
	.set	nomicromips
	.ent	testJoin
	.type	testJoin, @function
testJoin:
	.frame	$fp,48,$31		# vars= 24, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-48
	.cfi_def_cfa_offset 48
	sw	$31,44($sp)
	sw	$fp,40($sp)
	.cfi_offset 31, -4
	.cfi_offset 30, -8
	move	$fp,$sp
	.cfi_def_cfa_register 30
	.loc 1 124 0
	li	$2,57			# 0x39
	sw	$2,24($fp)
	li	$2,99			# 0x63
	sw	$2,28($fp)
	.loc 1 125 0
	addiu	$3,$fp,24
	lui	$2,%hi(threadFunction3)
	addiu	$4,$2,%lo(threadFunction3)
	move	$5,$3
	jal	UserThreadCreate
	nop

	sw	$2,16($fp)
	.loc 1 126 0
	lw	$2,16($fp)
	nop
	sw	$2,32($fp)
	.loc 1 127 0
	addiu	$3,$fp,32
	lui	$2,%hi(threadFunctionJoin)
	addiu	$4,$2,%lo(threadFunctionJoin)
	move	$5,$3
	jal	UserThreadCreate
	nop

	sw	$2,20($fp)
	.loc 1 129 0
	lw	$4,20($fp)
	jal	UserThreadJoin
	nop

	.loc 1 130 0
	move	$sp,$fp
	.cfi_def_cfa_register 29
	lw	$31,44($sp)
	lw	$fp,40($sp)
	addiu	$sp,$sp,48
	.cfi_restore 30
	.cfi_restore 31
	.cfi_def_cfa_offset 0
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	testJoin
	.cfi_endproc
$LFE7:
	.size	testJoin, .-testJoin
	.rdata
	.align	2
$LC3:
	.ascii	"Begin\012\000"
	.text
	.align	2
	.globl	main
$LFB8 = .
	.loc 1 135 0
	.cfi_startproc
	.set	nomips16
	.set	nomicromips
	.ent	main
	.type	main, @function
main:
	.frame	$fp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	.cfi_def_cfa_offset 24
	sw	$31,20($sp)
	sw	$fp,16($sp)
	.cfi_offset 31, -4
	.cfi_offset 30, -8
	move	$fp,$sp
	.cfi_def_cfa_register 30
	sw	$4,24($fp)
	sw	$5,28($fp)
	.loc 1 136 0
	jal	testCreation
	nop

	.loc 1 138 0
	lui	$2,%hi($LC3)
	addiu	$4,$2,%lo($LC3)
	li	$5,6			# 0x6
	jal	PutString
	nop

	.loc 1 139 0
	jal	Halt
	nop

	.set	macro
	.set	reorder
	.end	main
	.cfi_endproc
$LFE8:
	.size	main, .-main
$Letext0:
	.section	.debug_info,"",@progbits
$Ldebug_info0:
	.4byte	0x267
	.2byte	0x4
	.4byte	$Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	$LASF22
	.byte	0x1
	.4byte	$LASF23
	.4byte	$LASF24
	.4byte	$Ltext0
	.4byte	$Letext0-$Ltext0
	.4byte	$Ldebug_line0
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x3
	.4byte	$LASF18
	.byte	0x1
	.byte	0x12
	.4byte	0x25
	.4byte	$LFB0
	.4byte	$LFE0-$LFB0
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x70
	.uleb128 0x4
	.ascii	"str\000"
	.byte	0x1
	.byte	0x12
	.4byte	0x70
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x5
	.ascii	"res\000"
	.byte	0x1
	.byte	0x14
	.4byte	0x25
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x6
	.4byte	$LASF0
	.byte	0x1
	.byte	0x15
	.4byte	0x70
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.byte	0
	.uleb128 0x7
	.byte	0x4
	.4byte	0x76
	.uleb128 0x8
	.byte	0x1
	.byte	0x6
	.4byte	$LASF1
	.uleb128 0x9
	.4byte	$LASF2
	.byte	0x1
	.byte	0x22
	.4byte	$LFB1
	.4byte	$LFE1-$LFB1
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xa1
	.uleb128 0x4
	.ascii	"arg\000"
	.byte	0x1
	.byte	0x22
	.4byte	0xa1
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0xa
	.byte	0x4
	.uleb128 0x9
	.4byte	$LASF3
	.byte	0x1
	.byte	0x2f
	.4byte	$LFB2
	.4byte	$LFE2-$LFB2
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xc7
	.uleb128 0x4
	.ascii	"arg\000"
	.byte	0x1
	.byte	0x2f
	.4byte	0xa1
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x9
	.4byte	$LASF4
	.byte	0x1
	.byte	0x3a
	.4byte	$LFB3
	.4byte	$LFE3-$LFB3
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xf9
	.uleb128 0x4
	.ascii	"arg\000"
	.byte	0x1
	.byte	0x3a
	.4byte	0xa1
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x6
	.4byte	$LASF5
	.byte	0x1
	.byte	0x3c
	.4byte	0x25
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x9
	.4byte	$LASF6
	.byte	0x1
	.byte	0x43
	.4byte	$LFB4
	.4byte	$LFE4-$LFB4
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x139
	.uleb128 0x4
	.ascii	"arg\000"
	.byte	0x1
	.byte	0x43
	.4byte	0xa1
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x6
	.4byte	$LASF5
	.byte	0x1
	.byte	0x45
	.4byte	0x25
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x6
	.4byte	$LASF7
	.byte	0x1
	.byte	0x46
	.4byte	0x25
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.byte	0
	.uleb128 0x9
	.4byte	$LASF8
	.byte	0x1
	.byte	0x4e
	.4byte	$LFB5
	.4byte	$LFE5-$LFB5
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x16b
	.uleb128 0x4
	.ascii	"arg\000"
	.byte	0x1
	.byte	0x4e
	.4byte	0xa1
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x5
	.ascii	"tid\000"
	.byte	0x1
	.byte	0x50
	.4byte	0x25
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0xb
	.4byte	$LASF14
	.byte	0x1
	.byte	0x57
	.4byte	$LFB6
	.4byte	$LFE6-$LFB6
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x1b9
	.uleb128 0x6
	.4byte	$LASF9
	.byte	0x1
	.byte	0x59
	.4byte	0x1b9
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x6
	.4byte	$LASF10
	.byte	0x1
	.byte	0x5c
	.4byte	0x25
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x6
	.4byte	$LASF11
	.byte	0x1
	.byte	0x62
	.4byte	0x25
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x6
	.4byte	$LASF12
	.byte	0x1
	.byte	0x68
	.4byte	0x25
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0xc
	.4byte	0x25
	.4byte	0x1c9
	.uleb128 0xd
	.4byte	0x1c9
	.byte	0
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.byte	0x7
	.4byte	$LASF13
	.uleb128 0xb
	.4byte	$LASF15
	.byte	0x1
	.byte	0x7a
	.4byte	$LFB7
	.4byte	$LFE7-$LFB7
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x21e
	.uleb128 0x6
	.4byte	$LASF16
	.byte	0x1
	.byte	0x7c
	.4byte	0x21e
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x6
	.4byte	$LASF17
	.byte	0x1
	.byte	0x7d
	.4byte	0x25
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x6
	.4byte	$LASF5
	.byte	0x1
	.byte	0x7e
	.4byte	0x1b9
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x6
	.4byte	$LASF10
	.byte	0x1
	.byte	0x7f
	.4byte	0x25
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.byte	0
	.uleb128 0xc
	.4byte	0x25
	.4byte	0x22e
	.uleb128 0xd
	.4byte	0x1c9
	.byte	0x1
	.byte	0
	.uleb128 0xe
	.4byte	$LASF19
	.byte	0x1
	.byte	0x86
	.4byte	0x25
	.4byte	$LFB8
	.4byte	$LFE8-$LFB8
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x264
	.uleb128 0xf
	.4byte	$LASF20
	.byte	0x1
	.byte	0x86
	.4byte	0x25
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0xf
	.4byte	$LASF21
	.byte	0x1
	.byte	0x86
	.4byte	0x264
	.uleb128 0x2
	.byte	0x91
	.sleb128 4
	.byte	0
	.uleb128 0x7
	.byte	0x4
	.4byte	0x70
	.byte	0
	.section	.debug_abbrev,"",@progbits
$Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.4byte	0x1c
	.2byte	0x2
	.4byte	$Ldebug_info0
	.byte	0x4
	.byte	0
	.2byte	0
	.2byte	0
	.4byte	$Ltext0
	.4byte	$Letext0-$Ltext0
	.4byte	0
	.4byte	0
	.section	.debug_line,"",@progbits
$Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
$LASF14:
	.ascii	"testCreation\000"
$LASF18:
	.ascii	"strlen\000"
$LASF4:
	.ascii	"threadFunction3\000"
$LASF19:
	.ascii	"main\000"
$LASF8:
	.ascii	"threadFunctionJoin\000"
$LASF21:
	.ascii	"argv\000"
$LASF16:
	.ascii	"arg0\000"
$LASF17:
	.ascii	"tid0\000"
$LASF10:
	.ascii	"tid1\000"
$LASF11:
	.ascii	"tid2\000"
$LASF12:
	.ascii	"tid3\000"
$LASF20:
	.ascii	"argc\000"
$LASF2:
	.ascii	"threadFunction1\000"
$LASF3:
	.ascii	"threadFunction2\000"
$LASF24:
	.ascii	"/home/littlegirle/Desktop/khedma/Programming/WorkspaceC/"
	.ascii	"os_projectNACHOS/src/code/build\000"
$LASF6:
	.ascii	"threadFunction4\000"
$LASF15:
	.ascii	"testJoin\000"
$LASF22:
	.ascii	"GNU C 4.9.2 -G 0 -mips1 -mno-abicalls -mfp32 -mtune=mips"
	.ascii	"32 -mllsc -mno-shared -g -fno-builtin\000"
$LASF13:
	.ascii	"sizetype\000"
$LASF5:
	.ascii	"arg1\000"
$LASF7:
	.ascii	"arg2\000"
$LASF9:
	.ascii	"arg3\000"
$LASF0:
	.ascii	"buffer\000"
$LASF1:
	.ascii	"char\000"
$LASF23:
	.ascii	"../test/multithreadingTest.c\000"
	.ident	"GCC: ( 4.9.2-10) 4.9.2"
