Skip to content
Search or jump to…
Pull requests
Issues
Codespaces
Marketplace
Explore
 
@AlonPulver 
YardenBakish
/
CPU-Simulator
Public
Code
Issues
Pull requests
Actions
Projects
Security
Insights
CPU-Simulator/circle.asm
@YardenBakish
YardenBakish Create circle.asm
Latest commit 2c1db45 on Aug 27, 2021
 History
 1 contributor
38 lines (38 sloc)  1.73 KB

.word 0x100 30
                add $sp,$zero,$imm,1
                sll $sp, $sp, $imm, 11      # set $sp = 1 << 11 = 2048
                add $sp,$sp,$imm,-3         # set place for 3 items
                sw  $s2, $sp, $imm, 2       # save registers that will be re-writen
                sw  $s1, $sp, $imm, 1
                sw  $s0, $sp, $imm, 0
                lw  $s2,$imm,$zero,0x100   #get stored radius
                mul $s2,$s2,$s2,0          #save radius in power of 2
                add $s1,$imm,$zero,175     #store center position
                add $s0,$imm,$zero,143
                add $t0,$imm,$zero,0      #set x counter
nextx:          add $t1,$imm,$zero,0      #set y counter
nexty:          add $t2,$imm,$zero,352    #set loop exit
                beq $imm,$t0,$t2,exit     # jump to exit if we checked all positions
                sub $t2,$s1,$t0,0         # calculate distance from center in power of 2
                sub $t3,$s0,$t1,0
                mul $t2,$t2,$t2,0         # get the square of the sub between the coordinates
                mul $t3,$t3,$t3,0
                add $t3,$t3,$t2,0
                ble $imm,$t3,$s2,color    #check if we need to color it in white
aftercolor:     add $t1,$t1,$imm,1        # append y counter
                add $t3,$imm,$zero,288
                bne $imm,$t1,$t3,nexty    #check if we ran over all 288 options of y for this x. if not we jump to next y
                add $t0,$t0,$imm,1        # "else" we append to next x and start over the loop
                jal $imm,$zero,$zero,nextx
color:          out $t0,$zero,$imm,19      #set monitor parameters x , y and color
                out $t1,$zero,$imm,20
                add $t3,$imm,$zero,255
                out $t3,$zero,$imm,21
                add $t3,$imm,$zero,1
                out $t3,$zero,$imm,18
                jal $imm,$zero,$zero,aftercolor #jump back to check and append counters
exit:           lw $s0, $sp, $imm, 0        #restore registers and stack
                lw $s1, $sp, $imm, 1
                lw $s2, $sp, $imm, 2
                add $sp, $sp, $imm, 3
                halt $zero,$zero,$zero,0    
Footer
© 2022 GitHub, Inc.
Footer navigation
Terms
Privacy
Security
Status
Docs
Contact GitHub
Pricing
API
Training
Blog
About
YardenBakish/CPU-Simulator: Computer Architecture Project - Basic Assembler and Simulator programs for simplified MIPS architecture processorsCPU-Simulator/circle.asm at main · YardenBakish/CPU-Simulator