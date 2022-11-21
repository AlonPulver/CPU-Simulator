.word 0x100 5
.word 0x101 3


                
                add $sp,$zero,$imm,1
                sll $sp, $sp, $imm, 11      # set $sp = 1 << 11 = 2048
                add $sp,$sp,$imm,-2         # set place for 2 items
                sw $a1, $sp, $imm, 1        # save registers that will hold initial arguments
                sw $a0, $sp, $imm, 0
                lw $a1, $zero, $imm, 0x100  # get n from address 0x100
                lw $a0, $zero, $imm, 0x101  # get k from address 0x101
                jal $imm,$zero,$zero,binom  # jump to binom as if other program has called for it
                lw $a0, $sp, $imm, 0        # restore arguments and stack
                lw $a1, $sp, $imm, 1
                add $sp, $sp, $imm, 2
                sw $v0,$imm,$zero,0x102     # store the output in the right address
                halt $zero,$zero,$zero,0    # exit

binom:          add $sp,$sp,$imm,-4         # set place for 4 items
                sw $ra, $sp, $imm, 3        # save registers that will be re-writen with each call
                sw $s0, $sp, $imm, 2
                sw $a1, $sp, $imm, 1
                sw $a0, $sp, $imm, 0
                beq $imm,$a0,$zero,true     #check if k==0 
                beq $imm,$a0,$a1,true       #check if k==n
                beq $imm,$zero,$zero,else   # go to else if none of them are true
true:           add $v0,$imm,$zero,1        # set return value as 1
                beq $imm,$zero,$zero,return # jump to return without resetting the $ra return address

else:           add $a0,$a0,$imm,-1         #set new arguments 
                add $a1,$a1,$imm,-1
                jal $imm,$zero,$zero,binom  # call for binom(n-1,k-1)
                add $s0,$v0,$zero,0         # save binom(n-1,k-1) 
                add $a0,$a0,$imm,1       
                jal $imm,$zero,$zero,binom  #call for binom(n-1,k)
                add $v0,$v0,$s0,0           # save binom(n-1,k-1) +binom(n-1,k)

return:         lw $a0, $sp, $imm, 0        # restore registers
                lw $a1, $sp, $imm, 1
                lw $s0, $sp, $imm, 2
                lw $ra, $sp, $imm, 3
                add $sp,$sp,$imm,4
                beq $ra,$zero,$zero,0       # jump back to caller


