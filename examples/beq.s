
main:
    li   $t0, 12
    li   $t1, 5 
    # if t0 = t5, go to exit. otherwise continue. 
    beq  $t0, $t1, exit

second_half:

    sub  $a0, $t0, $t1
    addi $t1, $t1, 100
    
    li   $v0, 1 
    syscall 
    
    li   $a0, '\n'
    li   $v0, 11
    syscall

exit: 

    li $a0, 'A'
    li $v0, 11
    syscall

    li   $a0, '\n'
    li   $v0, 11
    syscall

    li $v0, 10
    syscall 
