	
	add $sp, $zero, $imm, 1		# set $sp = 1
	out $sp, $zero, $imm, 0		# enable irq0
	add $t0, $zero, $imm, 6		# set $t0 = 6
	out $imm, $t0, $zero, ON	# set irqhandler as ON
	add $s0, $zero, $imm, 10	# set $s0 = 10
	add $t0, $zero, $imm, 13	# set $t0 = 13
	out $imm, $t0, $zero, 255	# max timer = 255
	add $t0, $zero, $imm, 11	# set $t0 = 11
	out $imm, $t0, $zero, 1		# enable timer
	lw $s1, $zero, $imm, 0x100	# start time as 19:59:5400

SEC1:
	lw $t0, $zero, $imm, 0x101
	beq $imm, $s1, $t0, PREND	# check if to stop
	lw $t0, $zero, $imm, 0x108
	and $t1, $s1, $t0, 0		# isolate last sec 
	lw $t0, $zero, $imm, 0x102
	beq $imm, $t1, $t0, SEC2	# check if last sec digit is 9
	lw $t0, $zero, $imm, 0x10E
	add $s1, $s1, $t0, 0		# add to sec
	beq $imm, $zero, $zero, WAIT

SEC2:
	lw $t0, $zero, $imm, 0x114
	and $s1, $s1, $t0, 0		# reset last sec digit
	lw $t0, $zero, $imm, 0x109
	and $t1, $s1, $t0, 0		# isolate first sec 
	lw $t0, $zero, $imm, 0x103
	beq $imm, $t1, $t0, MIN1	# check if first sec digit is 5
	lw $t0, $zero, $imm, 0x10F
	add $s1, $s1, $t0, 0		# add to sec
	beq $imm, $zero, $zero, WAIT

MIN1:
	lw $t0, $zero, $imm, 0x115
	and $s1, $s1, $t0, 0		# reset first sec digit
	lw $t0, $zero, $imm, 0x10A
	and $t1, $s1, $t0, 0		# isolate last min 
	lw $t0, $zero, $imm, 0x104
	beq $imm, $t1, $t0, MIN2	# check if last min digit is 9
	lw $t0, $zero, $imm, 0x110
	add $s1, $s1, $t0, 0		# add to min
	beq $imm, $zero, $zero, WAIT

MIN2:
	lw $t0, $zero, $imm, 0x116
	and $s1, $s1, $t0, 0		# reset last min digit
	lw $t0, $zero, $imm, 0x10B
	and $t1, $s1, $t0, 0		# isolate first min 
	lw $t0, $zero, $imm, 0x105
	beq $imm, $t1, $t0, HOUR1	# check if first min digit is 5
	lw $t0, $zero, $imm, 0x111
	add $s1, $s1, $t0, 0		# add to min
	beq $imm, $zero, $zero, WAIT

HOUR1:
	lw $t0, $zero, $imm, 0x117
	and $s1, $s1, $t0, 0		# reset first min digit
	lw $t0, $zero, $imm, 0x10D
	and $t1, $s1, $t0, 0		# isolate first hour 
	lw $t0, $zero, $imm, 0x107
	beq $imm, $t1, $t0, HOUR2	# check if first hour digit is 2
	lw $t0, $zero, $imm, 0x10C
	and $t1, $s1, $t0, 0		# isolate last hour 
	lw $t0, $zero, $imm, 0x11A
	beq $imm, $t1, $t0, HOUR9	# check if last hour digit is 9
	lw $t0, $zero, $imm, 0x112
	add $s1, $s1, $t0, 0		# add to hour
	beq $imm, $zero, $zero, WAIT

HOUR9:
	lw $t0, $zero, $imm, 0x118
	and $s1, $s1, $t0, 0		# reset last hour digit
	lw $t0, $zero, $imm, 0x113
	add $s1, $s1, $t0, 0		# add to hour
	beq $imm, $zero, $zero, WAIT

HOUR2:
	lw $t0, $zero, $imm, 0x10C
	and $t1, $s1, $t0, 0		# isolate last hour 
	lw $t0, $zero, $imm, 0x106
	beq $imm, $t1, $t0, RESET # check if last hour digit is 4
	lw $t0, $zero, $imm, 0x112
	add $s1, $s1, $t0, 0		# add to hour
	beq $imm, $zero, $zero, WAIT

RESET:
	and $s1, $s1, $imm, 0		# reset all
	beq $imm, $zero, $zero, WAIT

ON:	
	out $s1, $s0, $zero, 0		# upadte time
	out $zero, $zero, $imm, 3	# clear irq0 status
	add $t2, $imm, $zero, 7
	out $imm, $zero, $t2, SEC1	# choose return adress
	reti $zero, $zero, $zero, 0	# return from interrupt

WAIT:
	beq $imm, $zero, $zero, WAIT	# wait for interrupt

PREND:
	add $t0, $zero, $imm, 6		# set $t0 = 6
	out $imm, $t0, $zero, END	# set irqhandler as END
	lw $t0, $zero, $imm, 0x10E
	add $s1, $s1, $t0, 0		# add to sec
	beq $imm, $zero, $zero, WAIT

END:
	out $s1, $s0, $zero, 0		# upadte time
	out $zero, $zero, $imm, 3	# clear irq0 status
	halt $zero, $zero, $zero, 0	# halt

	.word 0x100 0x19595400		# start time
	.word 0x101 0x20000400		# end time
	.word 0x102 0x00000900		# check first sec
	.word 0x103 0x00005000		# check second sec
	.word 0x104 0x00090000		# check first min 
	.word 0x105 0x00500000		# check second min
	.word 0x106 0x04000000		# check first hour 
	.word 0x107 0x20000000		# check second hour
	.word 0x108 0x00000f00		# needed to isolate sec
	.word 0x109 0x0000f000		# needed to isolate sec
	.word 0x10A 0x000f0000		# needed to isolate min
	.word 0x10B 0x00f00000		# needed to isolate min
	.word 0x10C 0x0f000000		# needed to isolate hour
	.word 0x10D 0xf0000000		# needed to isolate hour
	.word 0x10E 0x00000100		# needed to add sec
	.word 0x10F 0x00001000		# needed to add sec
	.word 0x110 0x00010000		# needed to add min
	.word 0x111 0x00100000		# needed to add min
	.word 0x112 0x01000000		# needed to add hour
	.word 0x113 0x10000000		# needed to add hour
	.word 0x114 0xFFFFF0FF		# needed to reset sec
	.word 0x115 0xFFFF0FFF		# needed to reset sec
	.word 0x116 0xFFF0FFFF		# needed to reset min
	.word 0x117 0xFF0FFFFF		# needed to reset min
	.word 0x118 0xF0FFFFFF		# needed to reset hour
	.word 0x119 0x0FFFFFFF		# needed to reset hour
	.word 0x11A 0x09000000		# check first hour 