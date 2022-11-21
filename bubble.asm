
	add $sp, $zero, $imm, 1			# set $sp = 1
	add $s0, $zero, $imm, 0x400		# adress of the array
	add $a0, $zero, $imm, 15
	add $s1, $zero, $imm, -1		# i=-1
	jal $imm, $zero, $zero, LOOPi
	halt $zero, $zero, $zero, 0		# halt

LOOPi:
	add $s1, $s1, $imm, 1			# i++
	bgt $ra, $s1, $a0, 0
	sub $t0, $a0, $s1, 0
	add $a1, $imm, $zero, 0
	
LOOPj:
	beq $imm, $a1, $t0, LOOPi
	lw $t1, $s0, $a1, 0				# load array [j] to $t1
	add $t2, $s0, $a1, 0			
	lw $t2, $t2, $imm, 1			# load array [j+1] to $t2
	ble $imm, $t1, $t2, NOSWAP		# if smaller do not swap
	sw $t2, $a1, $s0, 0				# new array[j] = old array [j+1]
	add $t3, $a1, $s0, 0			
	sw $t1, $t3, $imm, 1			# new array[j+1] = old array [j]

NOSWAP:
	add $a1, $a1, $imm, 1
	beq $imm, $zero, $zero, LOOPj
	
	.word 1024 100
	.word 1025 2
	.word 1026 3
	.word 1027 -4
	.word 1028 5
	.word 1029 6
	.word 1030 777
	.word 1031 8
	.word 1032 9
	.word 1033 10
	.word 1034 -11
	.word 1035 12
	.word 1036 13
	.word 1037 36
	.word 1038 -15
	.word 1039 100