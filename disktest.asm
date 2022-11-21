
	add $sp, $zero, $imm, 1		# set $sp = 1
	out $sp, $zero, $imm, 1		# enable irq1
	add $t0, $zero, $imm, 6		# set $t0 = 6
	out $imm, $t0, $zero, ON	# set irqhandler as ON
	add $t0, $zero, $imm, 16	# set $t0 = 16
	out $imm, $t0, $zero, 0x80	# buffer adress = 128
	add $s0, $zero, $imm, 15	# set $s0 = 15
	add $s1, $zero, $imm, 14	# set $s1 = 14
	add $a0, $zero, $imm, 0		# set index for sectors
	out $a0, $s0, $zero, 0		# sector = 0
	add $a0, $a0, $imm, 4		# update index 
	out $imm, $s1, $zero, 1		# read data

WAIT:
	beq $imm, $zero, $zero, WAIT	# wait for disk status to change

READ:
	out $a0, $s0, $zero, 0		# sector = index
	add $a0, $a0, $imm, 4		# update index
	out $imm, $s1, $zero, 1		# read data
	out $zero, $zero, $imm, 4	# clear irq1 status
	reti $zero, $zero, $zero, 0	# return from interrupt

WRITE:
	out $a0, $s0, $zero, 0		# sector = index
	sub $a0, $a0, $imm, 3		# update index
	out $imm, $s1, $zero, 2		# write data
	out $zero, $zero, $imm, 4	# clear irq1 status
	reti $zero, $zero, $zero, 0	# return from interrupt
	
ON:
	add $t0, $zero, $imm, 7	
	bge $imm, $a0, $t0, PREND	#check if to end
	add $t0, $zero, $imm, 4	
	bge $imm, $a0, $t0, WRITE	#check if to write
	blt $imm, $a0, $t0, READ	#check if to read

PREND:
	out $zero, $zero, $imm, 4	# clear irq1 status
	out $a0, $s0, $zero, 0		# sector = index
	out $imm, $s1, $zero, 2		# write data
	add $t0, $zero, $imm, 6	
	out $imm, $t0, $zero, END	# set irqhandler as END
	reti $zero, $zero, $zero, 0	# return from interrupt

END:
	out $zero, $zero, $imm, 4	# clear irq1 status
	halt $zero, $zero, $zero, 0	# halt