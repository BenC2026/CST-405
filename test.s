.data

.text
.globl main
main:
    # Allocate stack space
    addi $sp, $sp, -400

    # Declared x at offset 0
    # Declared y at offset 4
    # Declared z at offset 8
    li $t0, 10
    sw $t0, 0($sp)
    li $t0, 20
    sw $t0, 4($sp)
    lw $t0, 0($sp)
    lw $t1, 4($sp)
    add $t0, $t0, $t1
    sw $t0, 8($sp)
    lw $t0, 8($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    lw $t0, 8($sp)
    li $t1, 5
    add $t0, $t0, $t1
    sw $t0, 0($sp)
    lw $t0, 0($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    lw $t0, 0($sp)
    lw $t1, 4($sp)
    add $t0, $t0, $t1
    lw $t1, 8($sp)
    add $t0, $t0, $t1
    sw $t0, 4($sp)
    lw $t0, 4($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall

    # Exit program
    addi $sp, $sp, 400
    li $v0, 10
    syscall
