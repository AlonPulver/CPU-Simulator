	
	add $sp, $zero, $imm, 1			# set $sp = 1
	sll $sp, $sp, $imm, 9			# set $sp = 1 << 9 = 512
	add $s0, $zero, $imm, 0x100		# start of array
	add $a0, $zero, $imm, 0			# start index
	add $a1, $zero, $imm, 15		# end index
	jal $imm, $zero, $zero, SORT
	halt $zero, $zero, $zero, 0		# halt

SORT:
	add $sp, $sp, $imm, -5			# adjust stack for 2 items
	sw $s2, $sp, $imm, 4			# save argument
	sw $s0, $sp, $imm, 3			# save argument
	sw $a0, $sp, $imm, 2			# save argument
	sw $a1, $sp, $imm, 1			# save argument
	sw $ra, $sp, $zero, 0			# save argument
	add $s1, $a1, $zero, 0			# start index
	bge $imm, $a0, $s1, RELEASE		# check if to continue
	jal $imm, $zero, $zero, PART	# partition
	add $s2, $v0, $zero, 0
	lw $a0, $sp, $imm, 2
	add $a1, $s2, $imm, -1
	jal $imm, $zero, $zero, SORT	# lower section
	add $a0, $s2, $imm, 1
	lw $a1, $sp, $imm, 1
	jal $imm, $zero, $zero, SORT	# upper section

RELEASE:
	lw $s2, $sp, $imm, 4			# restore $s2
	lw $s0, $sp, $imm, 3			# restore $s0
	lw $a0, $sp, $imm, 2			# restore $a0
	lw $a1, $sp, $imm, 1			# restore $a1
	lw $ra, $sp, $zero, 0			# restore $ra
	add $sp, $sp, $imm, 5			# pop items from stack
	beq $ra, $zero, $zero, 0		# and return

PART:
	add $sp, $sp, $imm, -2			# adjust stack for 2 items
	sw $s2, $sp, $imm, 1			# save argument
	sw $ra, $sp, $zero, 0			# save argument
	add $t0, $a0, $zero, 0			# start index
	add $s2, $a1, $zero, 0			# end index
	add $t2, $s0, $s1, 0			# last value adress
	lw $t1, $t2, $zero, 0			# load last value
	add $s1, $t0, $imm, -1		
	add $t3, $t0, $zero, 0		
	add $v0, $s2, $imm, -1			# update pivot index
LOOP:
	bgt $imm, $t3, $v0, CHAN		
	add $t2, $s0, $t3, 0			# get adress
	lw $t2, $t2, $zero, 0			# load next value
	blt $imm, $t1, $t2, FORW		# check if A[i]>A[pivot]
	add $s1, $s1, $imm, 1			# i++
	add $a0, $s1, $zero, 0			# update start index
	add	$a1, $t3, $zero, 0			# update end index
	jal $imm, $zero, $zero, SWAP
	add $t3, $t3, $imm, 1
	beq $imm, $zero, $zero, LOOP
FORW:
	add $t3, $t3, $imm, 1
	beq $imm, $zero, $zero, LOOP
CHAN:
	add $a0, $s1, $imm, 1
	add $a1, $s2, $zero, 0
	add $v0, $a0, $zero, 0
	jal $imm, $zero, $zero, SWAP
	lw $ra, $sp, $zero, 0			# restore $ra
	lw $s2, $sp, $imm, 1			# restore $s2
	add $sp, $sp, $imm, 2			# pop items from stack
	beq $ra, $zero, $zero, 0		# and return

SWAP:
	add $sp, $sp, $imm, -2			# adjust stack for 2 items
	sw $t1, $sp, $imm, 1			# save argument
	sw $t3, $sp, $imm, 0			# save argument
	add $t2, $s0, $a0, 0
	lw $t0, $s0, $a0, 0
	add $t1, $s0, $a1, 0
	lw $t3, $t1, $zero, 0
	sw $t3, $t2, $zero, 0			# array[i] = array [j]
	sw $t0, $t1, $zero, 0			# array[j] = array [i]
	lw $t1, $sp, $imm, 1			# restore $t1
	lw $t3, $sp, $imm, 0			# restore $t3
	add $sp, $sp, $imm, 2			# pop items from stack
	beq $ra, $zero, $zero, 0		# and return


	.word 256 100
	.word 257 2
	.word 258 3
	.word 259 -4
	.word 260 5
	.word 261 6
	.word 262 777
	.word 263 8
	.word 264 9
	.word 265 10
	.word 266 -11
	.word 267 12
	.word 268 13
	.word 269 36
	.word 270 -15
	.word 271 100