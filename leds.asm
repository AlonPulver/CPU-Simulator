
            add $t1,$zero,$imm,1010    #1010 so we got time to check irqstatus and light the next one
            out $t1,$imm,$zero,13      #set max timer
            add $t0,$zero,$imm,0       #set led counter
            out $imm,$zero,$zero,1     #enable irq0
            add $t2,$zero,$imm,6
            out $imm,$t2,$zero,lightup #set handler for lighting up leds when its time
            add $t1,$zero,$imm,1010    #1010 so we got time to check irqstatus and light the next one
            out $t1,$imm,$zero,13      #set max timer
            add $t2,$zero,$imm,11
            add $t3,$zero,$imm,1
            out $t3,$zero,$imm,9      #light first led ,takes 2 cycles to light 
            out $imm,$t2,$zero,1      #takes 2 cycles to enable timer so over all the led was lit for 2 cycles here
            add $t0,$t0,$imm,1        #append led counter
            add $t3,$zero,$imm,32
wait:       bne $imm,$t3,$t0,wait     #wait here until all leds are lit
            halt $zero,$zero,$zero,0    #halt if they are lit

lightup:    in $t1,$imm,$zero,3       #check if status is 1 
            beq $imm,$t1,$zero,wait   #if not we jump back to wait
            in $t1,$imm,$zero,9       #load leds data and turn on the next bit                     
            sll $t1,$t1,$imm,1        #set place for the next bit that we are about to turn on
            add $t1,$t1,$imm,1       #turn on the next bit
            out $t1,$zero,$imm,9     #light next led by updating the leds data
            out $zero,$imm,$zero,3      #clear irq status
            add $t1,$imm,$zero,4       
            out $t1,$imm,$zero,12      #reset timer to pre calculated value
            add $t0,$t0,$imm,1       #append led counter
            add $zero,$zero,$zero,0  # this line does nothing more then adding 1 cycle to our cycle count
            reti $zero,$zero,$zero,0 # we added 1 cycle so we could get our interupt by exactly the right moment and then we return to wait