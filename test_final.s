.data
_str0:	.asciiz "Alice\n"
_str1:	.asciiz "Hello\n"
_flit0:	.float 3.14
_flit1:	.float 2.0
_flit2:	.float 1.1
_flit3:	.float 2.2
_flit4:	.float 3.3
gCount:	.word 0
gTotal:	.word 0
gScores:	.space 20
_bool_true:	.asciiz "true"
_bool_false:	.asciiz "false"
_heap_ptr:	.word _heap_mem
_heap_mem:	.space 4096

.text
.globl main
    j main

__print_float:
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    mtc1 $zero, $f15
    c.lt.s $f12, $f15
    bc1f __pfl_positive
    neg.s $f12, $f12
    li $v0, 11
    li $a0, 45
    syscall
__pfl_positive:
    trunc.w.s $f13, $f12
    mfc1 $a0, $f13
    li $v0, 1
    syscall
    cvt.s.w $f13, $f13
    sub.s $f12, $f12, $f13
    li $t0, 1000000
    mtc1 $t0, $f13
    cvt.s.w $f13, $f13
    mul.s $f12, $f12, $f13
    li $t0, 0x3F000000
    mtc1 $t0, $f13
    add.s $f12, $f12, $f13
    trunc.w.s $f12, $f12
    mfc1 $t0, $f12
    beqz $t0, __pfl_done
    addi $sp, $sp, -8
    li $t1, 6
    add $t2, $sp, 5
__pfl_fill:
    li $t3, 10
    div $t0, $t3
    mfhi $t4
    addi $t4, $t4, 48
    sb $t4, 0($t2)
    mflo $t0
    addi $t2, $t2, -1
    addi $t1, $t1, -1
    bnez $t1, __pfl_fill
    add $t2, $sp, 5
    li $t1, 6
__pfl_trim:
    lb $t3, 0($t2)
    bne $t3, 48, __pfl_print
    addi $t2, $t2, -1
    addi $t1, $t1, -1
    bnez $t1, __pfl_trim
    addi $sp, $sp, 8
    j __pfl_done
__pfl_print:
    li $v0, 11
    li $a0, 46
    syscall
    move $t2, $sp
__pfl_loop:
    beqz $t1, __pfl_end
    lb $a0, 0($t2)
    li $v0, 11
    syscall
    addi $t2, $t2, 1
    addi $t1, $t1, -1
    j __pfl_loop
__pfl_end:
    addi $sp, $sp, 8
__pfl_done:
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    jr $ra

__print_bool:
    beqz $a0, __pb_false
    la $a0, _bool_true
    j __pb_print
__pb_false:
    la $a0, _bool_false
__pb_print:
    li $v0, 4
    syscall
    jr $ra

    # Global 'gCount' lives in .data
    # Global 'gTotal' lives in .data
    # Global array 'gScores' lives in .data

# Function: main
main:
    addi $sp, $sp, -800
    # --- global variable initialization ---
    # --- end global init ---
    # Declared 'a' of type 'int' at offset 0
    # Declared 'b' of type 'int' at offset 4
    # Declared 'c' of type 'int' at offset 8
    li $t0, 42         # a = 42 (constant)
    sw $t0, 0($sp)     # store 'a'
    li $t1, 8         # b = 8 (constant)
    sw $t1, 4($sp)     # store 'b'
    lw $t0, 0($sp)     # load 'a'
    lw $t1, 4($sp)     # load 'b'
    sub $t2, $t0, $t1   # t0 = a - b
    move $t3, $t2       # c = t0
    sw $t3, 8($sp)     # store 'c'
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Print integer
    move $a0, $t1
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Print integer
    move $a0, $t3
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 'x' of type 'float' at offset 12
    # Declared 'y' of type 'float' at offset 16
    # Declared 'z' of type 'float' at offset 20
    l.s $f4, _flit0      # load float literal 3.14
    s.s $f4, 12($sp)     # store float 'x'
    l.s $f4, _flit1      # load float literal 2
    s.s $f4, 16($sp)     # store float 'y'
    l.s $f5, 12($sp)     # load float 'x'
    l.s $f6, 16($sp)     # load float 'y'
    mul.s $f4,$f5,$f6  # t0=x*y
    s.s $f4, 20($sp)     # store float 'z'
    # Print float
    mov.s $f12, $f5
    jal __print_float
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Print float
    mov.s $f12, $f6
    jal __print_float
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    l.s $f7, 20($sp)     # load float 'z'
    # Print float
    mov.s $f12, $f7
    jal __print_float
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 'flag' of type 'boolean' at offset 24
    # Declared 'done' of type 'boolean' at offset 28
    li $t0, 1         # flag = 1 (constant)
    sw $t0, 24($sp)     # store 'flag'
    li $t1, 0         # done = 0 (constant)
    sw $t1, 28($sp)     # store 'done'
    # Print boolean
    move $a0, $t0
    jal __print_bool
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Print boolean
    move $a0, $t1
    jal __print_bool
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 'grade' of type 'char' at offset 32
    # Declared 'init' of type 'char' at offset 36
    li $t0, 65         # grade = 65 (constant)
    sw $t0, 32($sp)     # store 'grade'
    li $t1, 90         # init = 90 (constant)
    sw $t1, 36($sp)     # store 'init'
    # Print char
    move $a0, $t0
    li $v0, 11
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Print char
    move $a0, $t1
    li $v0, 11
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 'name' of type 'string' at offset 40
    # Declared 'msg' of type 'string' at offset 44
    la $t2, _str0       # Load address of string literal
    move $t0, $t2       # name = t0
    sw $t0, 40($sp)     # store 'name'
    la $t1, _str1       # Load address of string literal
    move $t3, $t1       # msg = t1
    sw $t3, 44($sp)     # store 'msg'
    # Argument 0: name
    sw $t0, 600($sp)     # Store arg 0
    # output_string built-in
    lw $t4, 600($sp)     # Load string address from arg area
    move $a0, $t4
    li $v0, 4
    syscall
    # Argument 0: msg
    sw $t3, 600($sp)     # Store arg 0
    # output_string built-in
    lw $t4, 600($sp)     # Load string address from arg area
    move $a0, $t4
    li $v0, 4
    syscall
    # Declared array 'nums[6]' at offset 48
    li $t4, 0         # Load index constant
    # Array assign: nums[0] = 10
    sll $t4, $t4, 2    # index * 4
    addi $t5, $sp, 48   # base address of 'nums'
    add $t5, $t5, $t4 # element address
    li $t6, 10         # Load value constant
    sw $t6, 0($t5)     # store value
    li $t4, 1         # Load index constant
    # Array assign: nums[1] = 20
    sll $t4, $t4, 2    # index * 4
    addi $t5, $sp, 48   # base address of 'nums'
    add $t5, $t5, $t4 # element address
    li $t6, 20         # Load value constant
    sw $t6, 0($t5)     # store value
    li $t4, 2         # Load index constant
    # Array assign: nums[2] = 30
    sll $t4, $t4, 2    # index * 4
    addi $t5, $sp, 48   # base address of 'nums'
    add $t5, $t5, $t4 # element address
    li $t6, 30         # Load value constant
    sw $t6, 0($t5)     # store value
    li $t4, 3         # Load index constant
    # Array assign: nums[3] = 40
    sll $t4, $t4, 2    # index * 4
    addi $t5, $sp, 48   # base address of 'nums'
    add $t5, $t5, $t4 # element address
    li $t6, 40         # Load value constant
    sw $t6, 0($t5)     # store value
    li $t4, 4         # Load index constant
    # Array assign: nums[4] = 50
    sll $t4, $t4, 2    # index * 4
    addi $t5, $sp, 48   # base address of 'nums'
    add $t5, $t5, $t4 # element address
    li $t6, 50         # Load value constant
    sw $t6, 0($t5)     # store value
    li $t4, 5         # Load index constant
    # Array assign: nums[5] = 60
    sll $t4, $t4, 2    # index * 4
    addi $t5, $sp, 48   # base address of 'nums'
    add $t5, $t5, $t4 # element address
    li $t6, 60         # Load value constant
    sw $t6, 0($t5)     # store value
    li $t4, 0         # Load index constant
    # Array access: t2 = nums[0]
    move $t5, $t4    # copy index to scratch
    sll $t5, $t5, 2    # index * 4
    addi $t6, $sp, 48   # base address of 'nums'
    add $t6, $t6, $t5 # element address
    move $t7, $t6    # point to element
    lw $t7, 0($t7)     # load value
    # Print integer
    move $a0, $t7
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t4, 3         # Load index constant
    # Array access: t2 = nums[3]
    move $t5, $t4    # copy index to scratch
    sll $t5, $t5, 2    # index * 4
    addi $t6, $sp, 48   # base address of 'nums'
    add $t6, $t6, $t5 # element address
    move $t7, $t6    # point to element
    lw $t7, 0($t7)     # load value
    # Print integer
    move $a0, $t7
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t4, 5         # Load index constant
    # Array access: t2 = nums[5]
    move $t5, $t4    # copy index to scratch
    sll $t5, $t5, 2    # index * 4
    addi $t6, $sp, 48   # base address of 'nums'
    add $t6, $t6, $t5 # element address
    move $t7, $t6    # point to element
    lw $t7, 0($t7)     # load value
    # Print integer
    move $a0, $t7
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared array 'fArr[3]' at offset 72
    l.s $f8, _flit2      # load float literal 1.1
    li $t4, 0         # Load index constant
    # Array assign: fArr[0] = t2
    sll $t4, $t4, 2    # index * 4
    addi $t5, $sp, 72   # base address of 'fArr'
    add $t5, $t5, $t4 # element address
    s.s $f8, 0($t5)    # store float element
    l.s $f8, _flit3      # load float literal 2.2
    li $t4, 1         # Load index constant
    # Array assign: fArr[1] = t2
    sll $t4, $t4, 2    # index * 4
    addi $t5, $sp, 72   # base address of 'fArr'
    add $t5, $t5, $t4 # element address
    s.s $f8, 0($t5)    # store float element
    l.s $f8, _flit4      # load float literal 3.3
    li $t4, 2         # Load index constant
    # Array assign: fArr[2] = t2
    sll $t4, $t4, 2    # index * 4
    addi $t5, $sp, 72   # base address of 'fArr'
    add $t5, $t5, $t4 # element address
    s.s $f8, 0($t5)    # store float element
    li $t4, 0         # Load index constant
    # Array access: t2 = fArr[0]
    move $t5, $t4    # copy index to scratch
    sll $t5, $t5, 2    # index * 4
    addi $t6, $sp, 72   # base address of 'fArr'
    add $t6, $t6, $t5 # element address
    l.s $f8, 0($t6)    # load float element
    # Print float
    mov.s $f12, $f8
    jal __print_float
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t4, 2         # Load index constant
    # Array access: t2 = fArr[2]
    move $t5, $t4    # copy index to scratch
    sll $t5, $t5, 2    # index * 4
    addi $t6, $sp, 72   # base address of 'fArr'
    add $t6, $t6, $t5 # element address
    l.s $f8, 0($t6)    # load float element
    # Print float
    mov.s $f12, $f8
    jal __print_float
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Call function greet
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal greet
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t4, $v0
    # Declared 'magic' of type 'int' at offset 84
    # Call function getAnswer
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal getAnswer
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t5, $v0
    move $t6, $t5       # magic = t3
    sw $t6, 84($sp)     # store 'magic'
    # Print integer
    move $a0, $t6
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 'sq' of type 'int' at offset 88
    # Argument 0: 7
    li $t6, 7
    sw $t6, 600($sp)     # Store arg 0
    # Call function square
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal square
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t5, $v0
    move $t6, $t5       # sq = t3
    sw $t6, 88($sp)     # store 'sq'
    # Print integer
    move $a0, $t6
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 'avg' of type 'float' at offset 92
    # Argument 0: 10
    li $t6, 10
    sw $t6, 600($sp)     # Store arg 0
    # Argument 1: 3
    li $t6, 3
    sw $t6, 604($sp)     # Store arg 1
    # Call function average
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal average
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    mov.s $f9, $f0      # float return value
    s.s $f9, 92($sp)     # store float 'avg'
    l.s $f10, 92($sp)     # load float 'avg'
    # Print float
    mov.s $f12, $f10
    jal __print_float
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t6, 0         # gCount = 0 (constant)
    la $t8, gCount
    sw $t6, 0($t8)     # store global 'gCount'
    # Call function increment
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal increment
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t5, $v0
    # Call function increment
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal increment
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t6, $v0
    # Call function increment
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal increment
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t7, $v0
    la $t8, gCount
    lw $t8, 0($t8)     # load global 'gCount'
    # Print integer
    move $a0, $t8
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 'fact' of type 'int' at offset 96
    # Argument 0: 5
    li $t8, 5
    sw $t8, 600($sp)     # Store arg 0
    # Call function factorial
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal factorial
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t8, $v0
    move $t9, $t8       # fact = t6
    sw $t9, 96($sp)     # store 'fact'
    # Print integer
    move $a0, $t9
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared array 'arr[5]' at offset 100
    # Declared 's' of type 'int' at offset 120
    li $t9, 0         # Load index constant
    # Array assign: arr[0] = 1
    sll $t9, $t9, 2    # index * 4
    addi $t2, $sp, 100   # base address of 'arr'
    add $t2, $t2, $t9 # element address
    li $t1, 1         # Load value constant
    sw $t1, 0($t2)     # store value
    li $t1, 1         # Load index constant
    # Array assign: arr[1] = 2
    sll $t1, $t1, 2    # index * 4
    addi $t2, $sp, 100   # base address of 'arr'
    add $t2, $t2, $t1 # element address
    li $t9, 2         # Load value constant
    sw $t9, 0($t2)     # store value
    li $t1, 2         # Load index constant
    # Array assign: arr[2] = 3
    sll $t1, $t1, 2    # index * 4
    addi $t2, $sp, 100   # base address of 'arr'
    add $t2, $t2, $t1 # element address
    li $t9, 3         # Load value constant
    sw $t9, 0($t2)     # store value
    li $t1, 3         # Load index constant
    # Array assign: arr[3] = 4
    sll $t1, $t1, 2    # index * 4
    addi $t2, $sp, 100   # base address of 'arr'
    add $t2, $t2, $t1 # element address
    li $t9, 4         # Load value constant
    sw $t9, 0($t2)     # store value
    li $t1, 4         # Load index constant
    # Array assign: arr[4] = 5
    sll $t1, $t1, 2    # index * 4
    addi $t2, $sp, 100   # base address of 'arr'
    add $t2, $t2, $t1 # element address
    li $t9, 5         # Load value constant
    sw $t9, 0($t2)     # store value
    # Argument 0: arr
    addi $t1, $sp, 100   # Address of array 'arr'
    sw $t1, 600($sp)     # Store arg 0 (array address)
    # Argument 1: 5
    li $t1, 5
    sw $t1, 604($sp)     # Store arg 1
    # Call function sumArray
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal sumArray
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t8, $v0
    move $t1, $t8       # s = t6
    sw $t1, 120($sp)     # store 's'
    # Print integer
    move $a0, $t1
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 'even' of type 'boolean' at offset 124
    # Argument 0: 4
    li $t1, 4
    sw $t1, 600($sp)     # Store arg 0
    # Call function isEven
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal isEven
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t8, $v0
    move $t1, $t8       # even = t6
    sw $t1, 124($sp)     # store 'even'
    # Print boolean
    move $a0, $t1
    jal __print_bool
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Argument 0: 7
    li $t1, 7
    sw $t1, 600($sp)     # Store arg 0
    # Call function isEven
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal isEven
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t8, $v0
    move $t1, $t8       # even = t6
    sw $t1, 124($sp)     # store 'even'
    # Print boolean
    move $a0, $t1
    jal __print_bool
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 'i' of type 'int' at offset 128
    # Declared 'total' of type 'int' at offset 132
    li $t1, 1         # i = 1 (constant)
    sw $t1, 128($sp)     # store 'i'
    li $t2, 0         # total = 0 (constant)
    sw $t2, 132($sp)     # store 'total'
L0:                    # Label
    lw $t1, 128($sp)     # load 'i'
    li $t9, 10         # Load constant 10
    sle $t8, $t1, $t9   # t6 = i <= 10
    beqz $t8, L1       # Jump if false
    lw $t2, 132($sp)     # load 'total'
    lw $t1, 128($sp)     # load 'i'
    add $t8, $t2, $t1   # t6 = total + i
    move $t2, $t8       # total = t6
    sw $t2, 132($sp)     # store 'total'
    lw $t1, 128($sp)     # load 'i'
    li $t0, 1         # Load constant 1
    add $t8, $t1, $t0   # t6 = i + 1
    move $t1, $t8       # i = t6
    sw $t1, 128($sp)     # store 'i'
    j L0              # Jump
L1:                    # Label
    # Print integer
    move $a0, $t2
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 'j' of type 'int' at offset 136
    # Declared 'fsum' of type 'int' at offset 140
    li $t2, 0         # fsum = 0 (constant)
    sw $t2, 140($sp)     # store 'fsum'
    li $t3, 1         # j = 1 (constant)
    sw $t3, 136($sp)     # store 'j'
L2:                    # Label
    lw $t3, 136($sp)     # load 'j'
    li $t4, 5         # Load constant 5
    sle $t8, $t3, $t4   # t6 = j <= 5
    beqz $t8, L3       # Jump if false
    lw $t2, 140($sp)     # load 'fsum'
    lw $t3, 136($sp)     # load 'j'
    add $t8, $t2, $t3   # t6 = fsum + j
    move $t2, $t8       # fsum = t6
    sw $t2, 140($sp)     # store 'fsum'
    lw $t3, 136($sp)     # load 'j'
    li $t0, 1         # Load constant 1
    add $t8, $t3, $t0   # t6 = j + 1
    move $t3, $t8       # j = t6
    sw $t3, 136($sp)     # store 'j'
    j L2              # Jump
L3:                    # Label
    # Print integer
    move $a0, $t2
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared array 'data[5]' at offset 144
    li $t3, 0         # j = 0 (constant)
    sw $t3, 136($sp)     # store 'j'
L4:                    # Label
    lw $t3, 136($sp)     # load 'j'
    li $t4, 5         # Load constant 5
    slt $t8, $t3, $t4   # t6 = j < 5
    beqz $t8, L5       # Jump if false
    lw $t3, 136($sp)     # load 'j'
    lw $t3, 136($sp)     # load 'j'
    mul $t2, $t3, $t3   # t6 = j * j
    # Array assign: data[j] = t6
    sll $t3, $t3, 2    # index * 4
    addi $t8, $sp, 144   # base address of 'data'
    add $t8, $t8, $t3 # element address
    sw $t2, 0($t8)     # store value
    lw $t2, 136($sp)     # load 'j'
    li $t0, 1         # Load constant 1
    add $t3, $t2, $t0   # t6 = j + 1
    move $t2, $t3       # j = t6
    sw $t2, 136($sp)     # store 'j'
    j L4              # Jump
L5:                    # Label
    li $t2, 0         # j = 0 (constant)
    sw $t2, 136($sp)     # store 'j'
L6:                    # Label
    lw $t2, 136($sp)     # load 'j'
    li $t4, 5         # Load constant 5
    slt $t3, $t2, $t4   # t6 = j < 5
    beqz $t3, L7       # Jump if false
    lw $t2, 136($sp)     # Load index 'j'
    # Array access: t6 = data[j]
    move $t3, $t2    # copy index to scratch
    sll $t3, $t3, 2    # index * 4
    addi $t8, $sp, 144   # base address of 'data'
    add $t8, $t8, $t3 # element address
    move $t5, $t8    # point to element
    lw $t5, 0($t5)     # load value
    # Print integer
    move $a0, $t5
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    lw $t2, 136($sp)     # load 'j'
    li $t0, 1         # Load constant 1
    add $t3, $t2, $t0   # t6 = j + 1
    move $t2, $t3       # j = t6
    sw $t2, 136($sp)     # store 'j'
    j L6              # Jump
L7:                    # Label
    # Declared 'row' of type 'int' at offset 164
    # Declared 'col' of type 'int' at offset 168
    # Declared 'product' of type 'int' at offset 172
    li $t5, 1         # row = 1 (constant)
    sw $t5, 164($sp)     # store 'row'
L8:                    # Label
    lw $t5, 164($sp)     # load 'row'
    li $t8, 2         # Load constant 2
    sle $t3, $t5, $t8   # t6 = row <= 2
    beqz $t3, L9       # Jump if false
    li $t3, 1         # col = 1 (constant)
    sw $t3, 168($sp)     # store 'col'
L10:                    # Label
    lw $t3, 168($sp)     # load 'col'
    li $t6, 3         # Load constant 3
    sle $t7, $t3, $t6   # t6 = col <= 3
    beqz $t7, L11       # Jump if false
    lw $t5, 164($sp)     # load 'row'
    lw $t3, 168($sp)     # load 'col'
    mul $t7, $t5, $t3   # t6 = row * col
    move $t9, $t7       # product = t6
    sw $t9, 172($sp)     # store 'product'
    # Print integer
    move $a0, $t9
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    lw $t3, 168($sp)     # load 'col'
    li $t0, 1         # Load constant 1
    add $t7, $t3, $t0   # t6 = col + 1
    move $t3, $t7       # col = t6
    sw $t3, 168($sp)     # store 'col'
    j L10              # Jump
L11:                    # Label
    lw $t5, 164($sp)     # load 'row'
    li $t0, 1         # Load constant 1
    add $t7, $t5, $t0   # t6 = row + 1
    move $t5, $t7       # row = t6
    sw $t5, 164($sp)     # store 'row'
    j L8              # Jump
L9:                    # Label
    # Declared 'k' of type 'int' at offset 176
    # Declared 'acc' of type 'int' at offset 180
    li $t2, 1         # j = 1 (constant)
    sw $t2, 136($sp)     # store 'j'
L12:                    # Label
    lw $t2, 136($sp)     # load 'j'
    li $t6, 3         # Load constant 3
    sle $t7, $t2, $t6   # t6 = j <= 3
    beqz $t7, L13       # Jump if false
    li $t7, 0         # k = 0 (constant)
    sw $t7, 176($sp)     # store 'k'
    li $t9, 0         # acc = 0 (constant)
    sw $t9, 180($sp)     # store 'acc'
L14:                    # Label
    lw $t7, 176($sp)     # load 'k'
    lw $t2, 136($sp)     # load 'j'
    slt $t1, $t7, $t2   # t6 = k < j
    beqz $t1, L15       # Jump if false
    lw $t9, 180($sp)     # load 'acc'
    li $t0, 1         # Load constant 1
    add $t1, $t9, $t0   # t6 = acc + 1
    move $t9, $t1       # acc = t6
    sw $t9, 180($sp)     # store 'acc'
    lw $t7, 176($sp)     # load 'k'
    li $t0, 1         # Load constant 1
    add $t1, $t7, $t0   # t6 = k + 1
    move $t7, $t1       # k = t6
    sw $t7, 176($sp)     # store 'k'
    j L14              # Jump
L15:                    # Label
    # Print integer
    move $a0, $t9
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    lw $t2, 136($sp)     # load 'j'
    li $t0, 1         # Load constant 1
    add $t1, $t2, $t0   # t6 = j + 1
    move $t2, $t1       # j = t6
    sw $t2, 136($sp)     # store 'j'
    j L12              # Jump
L13:                    # Label
    # Declared 'val' of type 'int' at offset 184
    li $t9, 10         # val = 10 (constant)
    sw $t9, 184($sp)     # store 'val'
    lw $t9, 184($sp)     # load 'val'
    li $t4, 5         # Load constant 5
    sgt $t1, $t9, $t4   # t6 = val > 5
    beqz $t1, L17       # Jump if false
    li $t1, 100         # Load constant for print
    # Print integer
    move $a0, $t1
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
L17:                    # Label
    li $t9, 3         # val = 3 (constant)
    sw $t9, 184($sp)     # store 'val'
    lw $t9, 184($sp)     # load 'val'
    li $t4, 5         # Load constant 5
    sgt $t1, $t9, $t4   # t6 = val > 5
    beqz $t1, L18       # Jump if false
    li $t1, 200         # Load constant for print
    # Print integer
    move $a0, $t1
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L19              # Jump
L18:                    # Label
    li $t1, 201         # Load constant for print
    # Print integer
    move $a0, $t1
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
L19:                    # Label
    li $t9, 75         # val = 75 (constant)
    sw $t9, 184($sp)     # store 'val'
    lw $t9, 184($sp)     # load 'val'
    li $t1, 90         # Load constant 90
    sge $t8, $t9, $t1   # t6 = val >= 90
    beqz $t8, L20       # Jump if false
    li $t8, 300         # Load constant for print
    # Print integer
    move $a0, $t8
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L21              # Jump
L20:                    # Label
    lw $t9, 184($sp)     # load 'val'
    li $t8, 80         # Load constant 80
    sge $t3, $t9, $t8   # t6 = val >= 80
    beqz $t3, L22       # Jump if false
    li $t3, 301         # Load constant for print
    # Print integer
    move $a0, $t3
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L23              # Jump
L22:                    # Label
    lw $t9, 184($sp)     # load 'val'
    li $t3, 70         # Load constant 70
    sge $t5, $t9, $t3   # t6 = val >= 70
    beqz $t5, L24       # Jump if false
    li $t5, 302         # Load constant for print
    # Print integer
    move $a0, $t5
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L25              # Jump
L24:                    # Label
    li $t5, 303         # Load constant for print
    # Print integer
    move $a0, $t5
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
L25:                    # Label
L23:                    # Label
L21:                    # Label
    # Declared 'p' of type 'int' at offset 188
    # Declared 'q' of type 'int' at offset 192
    li $t5, 5         # p = 5 (constant)
    sw $t5, 188($sp)     # store 'p'
    li $t6, 10         # q = 10 (constant)
    sw $t6, 192($sp)     # store 'q'
    lw $t5, 188($sp)     # load 'p'
    lw $t6, 192($sp)     # load 'q'
    slt $t7, $t5, $t6   # t6 = p < q
    beqz $t7, L26       # Jump if false
    lw $t5, 188($sp)     # load 'p'
    li $t7, 0         # Load constant 0
    sgt $t0, $t5, $t7   # t6 = p > 0
    beqz $t0, L28       # Jump if false
    li $t0, 400         # Load constant for print
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L29              # Jump
L28:                    # Label
    li $t0, 401         # Load constant for print
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
L29:                    # Label
    j L27              # Jump
L26:                    # Label
    li $t0, 402         # Load constant for print
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
L27:                    # Label
    # Declared 'day' of type 'int' at offset 196
    li $t0, 3         # day = 3 (constant)
    sw $t0, 196($sp)     # store 'day'
    # Declared '__sw30' of type 'int' at offset 200
    move $t2, $t0       # __sw30 = day
    sw $t2, 200($sp)     # store '__sw30'
    lw $t2, 200($sp)     # load '__sw30'
    li $t4, 1         # Load constant 1
    seq $t1, $t2, $t4   # t6 = __sw30 == 1
    beqz $t1, L36       # Jump if false
    j L30              # Jump
L36:                    # Label
    lw $t2, 200($sp)     # load '__sw30'
    li $t1, 2         # Load constant 2
    seq $t8, $t2, $t1   # t6 = __sw30 == 2
    beqz $t8, L37       # Jump if false
    j L31              # Jump
L37:                    # Label
    lw $t2, 200($sp)     # load '__sw30'
    li $t8, 3         # Load constant 3
    seq $t9, $t2, $t8   # t6 = __sw30 == 3
    beqz $t9, L34       # Jump if false
    j L32              # Jump
L34:                    # Label
    j L33              # Jump
L30:                    # Label
    li $t9, 501         # Load constant for print
    # Print integer
    move $a0, $t9
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L35              # Jump
L31:                    # Label
    li $t9, 502         # Load constant for print
    # Print integer
    move $a0, $t9
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L35              # Jump
L32:                    # Label
    li $t9, 503         # Load constant for print
    # Print integer
    move $a0, $t9
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L35              # Jump
L33:                    # Label
    li $t9, 500         # Load constant for print
    # Print integer
    move $a0, $t9
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L35              # Jump
L35:                    # Label
    # Declared 'season' of type 'int' at offset 204
    li $t9, 2         # season = 2 (constant)
    sw $t9, 204($sp)     # store 'season'
    # Declared '__sw38' of type 'int' at offset 208
    move $t3, $t9       # __sw38 = season
    sw $t3, 208($sp)     # store '__sw38'
    lw $t3, 208($sp)     # load '__sw38'
    li $t4, 1         # Load constant 1
    seq $t6, $t3, $t4   # t6 = __sw38 == 1
    beqz $t6, L45       # Jump if false
    j L38              # Jump
L45:                    # Label
    lw $t3, 208($sp)     # load '__sw38'
    li $t1, 2         # Load constant 2
    seq $t6, $t3, $t1   # t6 = __sw38 == 2
    beqz $t6, L46       # Jump if false
    j L39              # Jump
L46:                    # Label
    lw $t3, 208($sp)     # load '__sw38'
    li $t8, 3         # Load constant 3
    seq $t6, $t3, $t8   # t6 = __sw38 == 3
    beqz $t6, L47       # Jump if false
    j L40              # Jump
L47:                    # Label
    lw $t3, 208($sp)     # load '__sw38'
    li $t6, 4         # Load constant 4
    seq $t5, $t3, $t6   # t6 = __sw38 == 4
    beqz $t5, L43       # Jump if false
    j L41              # Jump
L43:                    # Label
    j L42              # Jump
L38:                    # Label
L39:                    # Label
L40:                    # Label
    li $t5, 510         # Load constant for print
    # Print integer
    move $a0, $t5
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L44              # Jump
L41:                    # Label
    li $t5, 511         # Load constant for print
    # Print integer
    move $a0, $t5
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L44              # Jump
L42:                    # Label
    li $t5, 512         # Load constant for print
    # Print integer
    move $a0, $t5
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L44              # Jump
L44:                    # Label
    # Declared 'code' of type 'int' at offset 212
    li $t5, 99         # code = 99 (constant)
    sw $t5, 212($sp)     # store 'code'
    # Declared '__sw48' of type 'int' at offset 216
    move $t7, $t5       # __sw48 = code
    sw $t7, 216($sp)     # store '__sw48'
    lw $t7, 216($sp)     # load '__sw48'
    li $t4, 1         # Load constant 1
    seq $t0, $t7, $t4   # t6 = __sw48 == 1
    beqz $t0, L53       # Jump if false
    j L48              # Jump
L53:                    # Label
    lw $t7, 216($sp)     # load '__sw48'
    li $t1, 2         # Load constant 2
    seq $t0, $t7, $t1   # t6 = __sw48 == 2
    beqz $t0, L51       # Jump if false
    j L49              # Jump
L51:                    # Label
    j L50              # Jump
L48:                    # Label
    li $t0, 520         # Load constant for print
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L52              # Jump
L49:                    # Label
    li $t0, 521         # Load constant for print
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L52              # Jump
L50:                    # Label
    li $t0, 522         # Load constant for print
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L52              # Jump
L52:                    # Label
    # Declared 'outer' of type 'int' at offset 220
    # Declared 'inner' of type 'int' at offset 224
    li $t0, 2         # outer = 2 (constant)
    sw $t0, 220($sp)     # store 'outer'
    li $t2, 1         # inner = 1 (constant)
    sw $t2, 224($sp)     # store 'inner'
    # Declared '__sw54' of type 'int' at offset 228
    move $t9, $t0       # __sw54 = outer
    sw $t9, 228($sp)     # store '__sw54'
    lw $t9, 228($sp)     # load '__sw54'
    li $t4, 1         # Load constant 1
    seq $t8, $t9, $t4   # t6 = __sw54 == 1
    beqz $t8, L59       # Jump if false
    j L54              # Jump
L59:                    # Label
    lw $t9, 228($sp)     # load '__sw54'
    li $t1, 2         # Load constant 2
    seq $t8, $t9, $t1   # t6 = __sw54 == 2
    beqz $t8, L57       # Jump if false
    j L55              # Jump
L57:                    # Label
    j L56              # Jump
L54:                    # Label
    # Declared '__sw60' of type 'int' at offset 232
    move $t8, $t2       # __sw60 = inner
    sw $t8, 232($sp)     # store '__sw60'
    lw $t8, 232($sp)     # load '__sw60'
    li $t4, 1         # Load constant 1
    seq $t3, $t8, $t4   # t6 = __sw60 == 1
    beqz $t3, L62       # Jump if false
    j L60              # Jump
L62:                    # Label
    j L61              # Jump
L60:                    # Label
    li $t3, 531         # Load constant for print
    # Print integer
    move $a0, $t3
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L63              # Jump
L61:                    # Label
    li $t3, 530         # Load constant for print
    # Print integer
    move $a0, $t3
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L63              # Jump
L63:                    # Label
    j L58              # Jump
L55:                    # Label
    # Declared '__sw64' of type 'int' at offset 236
    move $t3, $t2       # __sw64 = inner
    sw $t3, 236($sp)     # store '__sw64'
    lw $t3, 236($sp)     # load '__sw64'
    li $t4, 1         # Load constant 1
    seq $t6, $t3, $t4   # t6 = __sw64 == 1
    beqz $t6, L66       # Jump if false
    j L64              # Jump
L66:                    # Label
    j L65              # Jump
L64:                    # Label
    li $t6, 532         # Load constant for print
    # Print integer
    move $a0, $t6
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L67              # Jump
L65:                    # Label
    li $t6, 530         # Load constant for print
    # Print integer
    move $a0, $t6
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L67              # Jump
L67:                    # Label
    j L58              # Jump
L56:                    # Label
    li $t6, 530         # Load constant for print
    # Print integer
    move $a0, $t6
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L58              # Jump
L58:                    # Label
    # Declared 'fibA' of type 'int' at offset 240
    # Declared 'fibB' of type 'int' at offset 244
    # Declared 'fibNext' of type 'int' at offset 248
    # Declared 'n' of type 'int' at offset 252
    li $t6, 0         # fibA = 0 (constant)
    sw $t6, 240($sp)     # store 'fibA'
    li $t5, 1         # fibB = 1 (constant)
    sw $t5, 244($sp)     # store 'fibB'
    # Print integer
    move $a0, $t6
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Print integer
    move $a0, $t5
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t5, 2         # n = 2 (constant)
    sw $t5, 252($sp)     # store 'n'
L68:                    # Label
    lw $t5, 252($sp)     # load 'n'
    li $t6, 7         # Load constant 7
    slt $t7, $t5, $t6   # t6 = n < 7
    beqz $t7, L69       # Jump if false
    lw $t7, 240($sp)     # load 'fibA'
    lw $t0, 244($sp)     # load 'fibB'
    add $t9, $t7, $t0   # t6 = fibA + fibB
    move $t1, $t9       # fibNext = t6
    sw $t1, 248($sp)     # store 'fibNext'
    # Print integer
    move $a0, $t1
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    move $t7, $t0       # fibA = fibB
    sw $t7, 240($sp)     # store 'fibA'
    lw $t1, 248($sp)     # load 'fibNext'
    move $t0, $t1       # fibB = fibNext
    sw $t0, 244($sp)     # store 'fibB'
    lw $t5, 252($sp)     # load 'n'
    li $t4, 1         # Load constant 1
    add $t9, $t5, $t4   # t6 = n + 1
    move $t5, $t9       # n = t6
    sw $t5, 252($sp)     # store 'n'
    j L68              # Jump
L69:                    # Label
    # Declared 'fibR' of type 'int' at offset 256
    # Argument 0: 6
    li $t8, 6
    sw $t8, 600($sp)     # Store arg 0
    # Call function fib
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal fib
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t9, $v0
    move $t8, $t9       # fibR = t6
    sw $t8, 256($sp)     # store 'fibR'
    # Print integer
    move $a0, $t8
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 'perf' of type 'int' at offset 260
    li $t8, 2         # Load constant 2
    li $t2, 3         # Load constant 3
    mul $t9, $t8, $t2   # t6 = 2 * 3
    li $t3, 4         # Load constant 4
    li $t6, 5         # Load constant 5
    mul $t7, $t3, $t6   # t0 = 4 * 5
    add $t1, $t9, $t7   # t7 = t6 + t0
    li $t4, 1         # Load constant 1
    sub $t7, $t1, $t4   # t0 = t7 - 1
    move $t0, $t7       # perf = t0
    sw $t0, 260($sp)     # store 'perf'
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 'limit' of type 'int' at offset 264
    # Declared 'lcount' of type 'int' at offset 268
    li $t0, 100         # limit = 100 (constant)
    sw $t0, 264($sp)     # store 'limit'
    li $t5, 0         # lcount = 0 (constant)
    sw $t5, 268($sp)     # store 'lcount'
    li $t8, 0         # i = 0 (constant)
    sw $t8, 128($sp)     # store 'i'
L70:                    # Label
    lw $t8, 128($sp)     # load 'i'
    lw $t0, 264($sp)     # load 'limit'
    slt $t7, $t8, $t0   # t0 = i < limit
    beqz $t7, L71       # Jump if false
    lw $t5, 268($sp)     # load 'lcount'
    li $t4, 1         # Load constant 1
    add $t7, $t5, $t4   # t0 = lcount + 1
    move $t5, $t7       # lcount = t0
    sw $t5, 268($sp)     # store 'lcount'
    lw $t8, 128($sp)     # load 'i'
    li $t4, 1         # Load constant 1
    add $t7, $t8, $t4   # t0 = i + 1
    move $t8, $t7       # i = t0
    sw $t8, 128($sp)     # store 'i'
    j L70              # Jump
L71:                    # Label
    # Print integer
    move $a0, $t5
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 'dc' of type 'int' at offset 272
    li $t5, 1         # dc = 1 (constant)
    sw $t5, 272($sp)     # store 'dc'
    lw $t5, 272($sp)     # load 'dc'
    li $t4, 1         # Load constant 1
    seq $t7, $t5, $t4   # t0 = dc == 1
    beqz $t7, L72       # Jump if false
    li $t7, 601         # Load constant for print
    # Print integer
    move $a0, $t7
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    j L73              # Jump
L72:                    # Label
    li $t7, 602         # Load constant for print
    # Print integer
    move $a0, $t7
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
L73:                    # Label
    li $t7, 0         # Load return value
    move $v0, $t7       # Move to return register
    addi $sp, $sp, 800
    li $v0, 10
    syscall
    # End of function main
    addi $sp, $sp, 800
    li $v0, 10
    syscall

# Function: greet
greet:
    addi $sp, $sp, -800
    sw $ra, 796($sp)
    li $t0, 999         # Load constant for print
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # End of function greet
    lw $ra, 796($sp)
    addi $sp, $sp, 800
    jr $ra

# Function: getAnswer
getAnswer:
    addi $sp, $sp, -800
    sw $ra, 796($sp)
    li $t0, 42         # Load return value
    move $v0, $t0       # Move to return register
    lw $ra, 796($sp)     # Restore return address
    addi $sp, $sp, 800
    jr $ra               # Return from function
    # End of function getAnswer
    lw $ra, 796($sp)
    addi $sp, $sp, 800
    jr $ra

# Function: square
square:
    addi $sp, $sp, -800
    sw $ra, 796($sp)
    lw $t0, 1400($sp)      # Load parameter 'n' from caller arg 0
    sw $t0, 0($sp)      # Store parameter 'n'
    # Declared 'result' of type 'int' at offset 4
    lw $t0, 0($sp)     # load 'n'
    lw $t0, 0($sp)     # load 'n'
    mul $t1, $t0, $t0   # t0 = n * n
    move $t2, $t1       # result = t0
    sw $t2, 4($sp)     # store 'result'
    move $v0, $t2       # Move to return register
    lw $ra, 796($sp)     # Restore return address
    addi $sp, $sp, 800
    jr $ra               # Return from function
    # End of function square
    lw $ra, 796($sp)
    addi $sp, $sp, 800
    jr $ra

# Function: average
average:
    addi $sp, $sp, -800
    sw $ra, 796($sp)
    lw $t0, 1400($sp)      # Load parameter 'numerator' from caller arg 0
    sw $t0, 0($sp)      # Store parameter 'numerator'
    lw $t1, 1404($sp)      # Load parameter 'denominator' from caller arg 1
    sw $t1, 4($sp)      # Store parameter 'denominator'
    # Declared 'r' of type 'float' at offset 8
    lw $t0, 0($sp)     # load 'numerator'
    lw $t1, 4($sp)     # load 'denominator'
    div $t0, $t1         # numerator / denominator
    mflo $t2              # t0 = quotient
    mtc1 $t2, $f4      # int->float convert
    cvt.s.w $f4, $f4
    s.s $f4, 8($sp)     # store float 'r'
    l.s $f5, 8($sp)     # load float 'r'
    mov.s $f0, $f5      # float return value
    lw $ra, 796($sp)     # Restore return address
    addi $sp, $sp, 800
    jr $ra               # Return from function
    # End of function average
    lw $ra, 796($sp)
    addi $sp, $sp, 800
    jr $ra

# Function: increment
increment:
    addi $sp, $sp, -800
    sw $ra, 796($sp)
    la $t8, gCount
    lw $t0, 0($t8)     # load global 'gCount'
    li $t1, 1         # Load constant 1
    add $t2, $t0, $t1   # t0 = gCount + 1
    move $t0, $t2       # gCount = t0
    la $t8, gCount
    sw $t0, 0($t8)     # store global 'gCount'
    # End of function increment
    lw $ra, 796($sp)
    addi $sp, $sp, 800
    jr $ra

# Function: factorial
factorial:
    addi $sp, $sp, -800
    sw $ra, 796($sp)
    lw $t0, 1400($sp)      # Load parameter 'n' from caller arg 0
    sw $t0, 0($sp)      # Store parameter 'n'
    lw $t0, 0($sp)     # load 'n'
    li $t1, 1         # Load constant 1
    sle $t2, $t0, $t1   # t0 = n <= 1
    beqz $t2, L75       # Jump if false
    li $t1, 1         # Load return value
    move $v0, $t1       # Move to return register
    lw $ra, 796($sp)     # Restore return address
    addi $sp, $sp, 800
    jr $ra               # Return from function
L75:                    # Label
    # Declared 'sub' of type 'int' at offset 4
    lw $t0, 0($sp)     # load 'n'
    li $t1, 1         # Load constant 1
    sub $t2, $t0, $t1   # t0 = n - 1
    move $t3, $t2       # sub = t0
    sw $t3, 4($sp)     # store 'sub'
    # Argument 0: sub
    sw $t3, 600($sp)     # Store arg 0
    # Call function factorial
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal factorial
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t2, $v0
    lw $t0, 0($sp)     # load 'n'
    mul $t4, $t0, $t2   # t7 = n * t0
    move $v0, $t4       # Move to return register
    lw $ra, 796($sp)     # Restore return address
    addi $sp, $sp, 800
    jr $ra               # Return from function
    # End of function factorial
    lw $ra, 796($sp)
    addi $sp, $sp, 800
    jr $ra

# Function: sumArray
sumArray:
    addi $sp, $sp, -800
    sw $ra, 796($sp)
    lw $t0, 1400($sp)      # Load parameter 'arr' from caller arg 0
    sw $t0, 0($sp)      # Store parameter 'arr'
    lw $t1, 1404($sp)      # Load parameter 'size' from caller arg 1
    sw $t1, 4($sp)      # Store parameter 'size'
    # Declared 'total' of type 'int' at offset 8
    # Declared 'idx' of type 'int' at offset 12
    li $t2, 0         # total = 0 (constant)
    sw $t2, 8($sp)     # store 'total'
    li $t3, 0         # idx = 0 (constant)
    sw $t3, 12($sp)     # store 'idx'
L76:                    # Label
    lw $t3, 12($sp)     # load 'idx'
    lw $t1, 4($sp)     # load 'size'
    slt $t4, $t3, $t1   # t7 = idx < size
    beqz $t4, L77       # Jump if false
    lw $t3, 12($sp)     # Load index 'idx'
    # Array access: t7 = arr[idx]
    move $t4, $t3    # copy index to scratch
    sll $t4, $t4, 2    # index * 4
    lw $t5, 0($sp)     # load pointer 'arr'
    add $t5, $t5, $t4 # element address
    move $t6, $t5    # point to element
    lw $t6, 0($t6)     # load value
    lw $t2, 8($sp)     # load 'total'
    add $t3, $t2, $t6   # t0 = total + t7
    move $t2, $t3       # total = t0
    sw $t2, 8($sp)     # store 'total'
    lw $t4, 12($sp)     # load 'idx'
    li $t5, 1         # Load constant 1
    add $t6, $t4, $t5   # t7 = idx + 1
    move $t4, $t6       # idx = t7
    sw $t4, 12($sp)     # store 'idx'
    j L76              # Jump
L77:                    # Label
    move $v0, $t2       # Move to return register
    lw $ra, 796($sp)     # Restore return address
    addi $sp, $sp, 800
    jr $ra               # Return from function
    # End of function sumArray
    lw $ra, 796($sp)
    addi $sp, $sp, 800
    jr $ra

# Function: isEven
isEven:
    addi $sp, $sp, -800
    sw $ra, 796($sp)
    lw $t0, 1400($sp)      # Load parameter 'n' from caller arg 0
    sw $t0, 0($sp)      # Store parameter 'n'
    lw $t0, 0($sp)     # load 'n'
    li $t1, 2         # Load constant 2
    div $t0, $t1         # n / 2
    mflo $t2              # t7 = quotient
    li $t1, 2         # Load constant 2
    mul $t3, $t2, $t1   # t0 = t7 * 2
    lw $t0, 0($sp)     # load 'n'
    seq $t2, $t0, $t3   # t7 = n == t0
    beqz $t2, L79       # Jump if false
    li $t2, 1         # Load return value
    move $v0, $t2       # Move to return register
    lw $ra, 796($sp)     # Restore return address
    addi $sp, $sp, 800
    jr $ra               # Return from function
L79:                    # Label
    li $t2, 0         # Load return value
    move $v0, $t2       # Move to return register
    lw $ra, 796($sp)     # Restore return address
    addi $sp, $sp, 800
    jr $ra               # Return from function
    # End of function isEven
    lw $ra, 796($sp)
    addi $sp, $sp, 800
    jr $ra

# Function: fib
fib:
    addi $sp, $sp, -800
    sw $ra, 796($sp)
    lw $t0, 1400($sp)      # Load parameter 'n' from caller arg 0
    sw $t0, 0($sp)      # Store parameter 'n'
    lw $t0, 0($sp)     # load 'n'
    li $t1, 0         # Load constant 0
    sle $t2, $t0, $t1   # t7 = n <= 0
    beqz $t2, L81       # Jump if false
    li $t1, 0         # Load return value
    move $v0, $t1       # Move to return register
    lw $ra, 796($sp)     # Restore return address
    addi $sp, $sp, 800
    jr $ra               # Return from function
L81:                    # Label
    lw $t0, 0($sp)     # load 'n'
    li $t1, 1         # Load constant 1
    seq $t2, $t0, $t1   # t7 = n == 1
    beqz $t2, L83       # Jump if false
    li $t1, 1         # Load return value
    move $v0, $t1       # Move to return register
    lw $ra, 796($sp)     # Restore return address
    addi $sp, $sp, 800
    jr $ra               # Return from function
L83:                    # Label
    # Declared 'a' of type 'int' at offset 4
    # Declared 'b' of type 'int' at offset 8
    lw $t0, 0($sp)     # load 'n'
    li $t1, 1         # Load constant 1
    sub $t2, $t0, $t1   # t7 = n - 1
    move $t3, $t2       # a = t7
    sw $t3, 4($sp)     # store 'a'
    lw $t0, 0($sp)     # load 'n'
    li $t4, 2         # Load constant 2
    sub $t2, $t0, $t4   # t7 = n - 2
    move $t5, $t2       # b = t7
    sw $t5, 8($sp)     # store 'b'
    # Argument 0: a
    sw $t3, 600($sp)     # Store arg 0
    # Call function fib
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal fib
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t2, $v0
    # Argument 0: b
    sw $t5, 600($sp)     # Store arg 0
    # Call function fib
    sw $t0, 500($sp)
    sw $t1, 504($sp)
    sw $t2, 508($sp)
    sw $t3, 512($sp)
    sw $t4, 516($sp)
    sw $t5, 520($sp)
    sw $t6, 524($sp)
    sw $t7, 528($sp)
    sw $t8, 532($sp)
    sw $t9, 536($sp)
    jal fib
    lw $t0, 500($sp)
    lw $t1, 504($sp)
    lw $t2, 508($sp)
    lw $t3, 512($sp)
    lw $t4, 516($sp)
    lw $t5, 520($sp)
    lw $t6, 524($sp)
    lw $t7, 528($sp)
    lw $t8, 532($sp)
    lw $t9, 536($sp)
    move $t6, $v0
    add $t7, $t2, $t6   # t6 = t7 + t0
    move $v0, $t7       # Move to return register
    lw $ra, 796($sp)     # Restore return address
    addi $sp, $sp, 800
    jr $ra               # Return from function
    # End of function fib
    lw $ra, 796($sp)
    addi $sp, $sp, 800
    jr $ra

    # Spill any remaining registers

    # Exit program
    addi $sp, $sp, 800
    li $v0, 10
    syscall
