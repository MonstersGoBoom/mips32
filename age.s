.mips32
; NOTE: You will have to hit cancel when Chrome complains that you are stuck in an infinite loop.
; Registers in use:
;   $t0: location of 'What year were you born?' on stack
;   $t1: location of 'Your age is:' on stack
;   $t2: location of 'Would you like to try again?' on stack
;   $t4: the current year (2013)

addi $t4, $zero, 0x1234
sw $t4,0x0000($a0)
addi $t4, $zero, 0x5678
sw $t4,0x0004($a0)

addi $t4, $zero, 2019


; Store 'What year were you born?' at the top of the stack
addiu $sp, $sp, -25
addi $t9, $zero, 87 ; 'W'
sb $t9, 0($sp)
addi $t9, $zero, 104 ; 'h'
sb $t9, 1($sp)
addi $t9, $zero, 97 ; 'a'
sb $t9, 2($sp)
addi $t9, $zero, 116 ; 't'
sb $t9, 3($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 4($sp)
addi $t9, $zero, 121 ; 'y'
sb $t9, 5($sp)
addi $t9, $zero, 101 ; 'e'
sb $t9, 6($sp)
addi $t9, $zero, 97 ; 'a'
sb $t9, 7($sp)
addi $t9, $zero, 114 ; 'r'
sb $t9, 8($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 9($sp)
addi $t9, $zero, 119 ; 'w'
sb $t9, 10($sp)
addi $t9, $zero, 101 ; 'e'
sb $t9, 11($sp)
addi $t9, $zero, 114 ; 'r'
sb $t9, 12($sp)
addi $t9, $zero, 101 ; 'e'
sb $t9, 13($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 14($sp)
addi $t9, $zero, 121 ; 'y'
sb $t9, 15($sp)
addi $t9, $zero, 111 ; 'o'
sb $t9, 16($sp)
addi $t9, $zero, 117 ; 'u'
sb $t9, 17($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 18($sp)
addi $t9, $zero, 98 ; 'b'
sb $t9, 19($sp)
addi $t9, $zero, 111 ; 'o'
sb $t9, 20($sp)
addi $t9, $zero, 114 ; 'r'
sb $t9, 21($sp)
addi $t9, $zero, 110 ; 'n'
sb $t9, 22($sp)
addi $t9, $zero, 63 ; '?'
sb $t9, 23($sp)
sb $zero, 24($sp) ; ''

addiu $t0, $sp, 0

; Store 'Your age is: ' at the top of the stack
addiu $sp, $sp, -14
addi $t9, $zero, 89 ; 'Y'
sb $t9, 0($sp)
addi $t9, $zero, 111 ; 'o'
sb $t9, 1($sp)
addi $t9, $zero, 117 ; 'u'
sb $t9, 2($sp)
addi $t9, $zero, 114 ; 'r'
sb $t9, 3($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 4($sp)
addi $t9, $zero, 97 ; 'a'
sb $t9, 5($sp)
addi $t9, $zero, 103 ; 'g'
sb $t9, 6($sp)
addi $t9, $zero, 101 ; 'e'
sb $t9, 7($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 8($sp)
addi $t9, $zero, 105 ; 'i'
sb $t9, 9($sp)
addi $t9, $zero, 115 ; 's'
sb $t9, 10($sp)
addi $t9, $zero, 58 ; ':'
sb $t9, 11($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 12($sp)
sb $zero, 13($sp) ; ''

addiu $t1, $sp, 0

; Store 'Would you like to try again?' at the top of the stack
addiu $sp, $sp, -29
addi $t9, $zero, 87 ; 'W'
sb $t9, 0($sp)
addi $t9, $zero, 111 ; 'o'
sb $t9, 1($sp)
addi $t9, $zero, 117 ; 'u'
sb $t9, 2($sp)
addi $t9, $zero, 108 ; 'l'
sb $t9, 3($sp)
addi $t9, $zero, 100 ; 'd'
sb $t9, 4($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 5($sp)
addi $t9, $zero, 121 ; 'y'
sb $t9, 6($sp)
addi $t9, $zero, 111 ; 'o'
sb $t9, 7($sp)
addi $t9, $zero, 117 ; 'u'
sb $t9, 8($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 9($sp)
addi $t9, $zero, 108 ; 'l'
sb $t9, 10($sp)
addi $t9, $zero, 105 ; 'i'
sb $t9, 11($sp)
addi $t9, $zero, 107 ; 'k'
sb $t9, 12($sp)
addi $t9, $zero, 101 ; 'e'
sb $t9, 13($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 14($sp)
addi $t9, $zero, 116 ; 't'
sb $t9, 15($sp)
addi $t9, $zero, 111 ; 'o'
sb $t9, 16($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 17($sp)
addi $t9, $zero, 116 ; 't'
sb $t9, 18($sp)
addi $t9, $zero, 114 ; 'r'
sb $t9, 19($sp)
addi $t9, $zero, 121 ; 'y'
sb $t9, 20($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 21($sp)
addi $t9, $zero, 97 ; 'a'
sb $t9, 22($sp)
addi $t9, $zero, 103 ; 'g'
sb $t9, 23($sp)
addi $t9, $zero, 97 ; 'a'
sb $t9, 24($sp)
addi $t9, $zero, 105 ; 'i'
sb $t9, 25($sp)
addi $t9, $zero, 110 ; 'n'
sb $t9, 26($sp)
addi $t9, $zero, 63 ; '?'
sb $t9, 27($sp)
sb $zero, 28($sp) ; ''

addiu $t2, $sp, 0

ASK_QUESTION:

addi $a0, $t0, 0
addi $v0, $zero, 51
syscall

beq $a1, $zero, VALID_VALUE

INVALID_VALUE:

; Store 'Invalid value entered.' at the top of the stack
addiu $sp, $sp, -23
addi $t9, $zero, 73 ; 'I'
sb $t9, 0($sp)
addi $t9, $zero, 110 ; 'n'
sb $t9, 1($sp)
addi $t9, $zero, 118 ; 'v'
sb $t9, 2($sp)
addi $t9, $zero, 97 ; 'a'
sb $t9, 3($sp)
addi $t9, $zero, 108 ; 'l'
sb $t9, 4($sp)
addi $t9, $zero, 105 ; 'i'
sb $t9, 5($sp)
addi $t9, $zero, 100 ; 'd'
sb $t9, 6($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 7($sp)
addi $t9, $zero, 118 ; 'v'
sb $t9, 8($sp)
addi $t9, $zero, 97 ; 'a'
sb $t9, 9($sp)
addi $t9, $zero, 108 ; 'l'
sb $t9, 10($sp)
addi $t9, $zero, 117 ; 'u'
sb $t9, 11($sp)
addi $t9, $zero, 101 ; 'e'
sb $t9, 12($sp)
addi $t9, $zero, 32 ; ' '
sb $t9, 13($sp)
addi $t9, $zero, 101 ; 'e'
sb $t9, 14($sp)
addi $t9, $zero, 110 ; 'n'
sb $t9, 15($sp)
addi $t9, $zero, 116 ; 't'
sb $t9, 16($sp)
addi $t9, $zero, 101 ; 'e'
sb $t9, 17($sp)
addi $t9, $zero, 114 ; 'r'
sb $t9, 18($sp)
addi $t9, $zero, 101 ; 'e'
sb $t9, 19($sp)
addi $t9, $zero, 100 ; 'd'
sb $t9, 20($sp)
addi $t9, $zero, 46 ; '.'
sb $t9, 21($sp)
sb $zero, 22($sp) ; ''

addi $a0, $sp, 0
addi $v0, $zero, 4 ; print string
syscall

  j END


VALID_VALUE:

; show an alert with their age

subu $t3, $t4, $a0 ; user's integer input

addi $a0, $t1, 0 ; 'your age is: '
addi $a1, $t3, 0 ; their age
addi $v0, $zero, 56
syscall


addi $a0, $t2, 0 ; 'try again?'
addi $v0, $zero, 50
syscall

beq $a0, $zero, ASK_QUESTION

END: