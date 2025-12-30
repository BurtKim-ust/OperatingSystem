# ECE 357 Pset 7 Problem 2 
# Burt Kim
# reference: https://filippo.io/linux-syscall-table/
#            https://cs.lmu.edu/~ray/notes/x86assembly/

.comm buf, 4096, 4
.text
.globl _start
_start:
loop:
    # read(0, buf, 4096)
    movq $0, %rax
    movq $0, %rdi        
    movq $buf, %rsi
    movq $4096, %rdx 
    syscall
    
    # if n <= 0, exit
    cmpq $0, %rax
    jle exit
    movq %rax, %r12
    
    # write(1, buf, n)
    movq $1, %rax
    movq $1, %rdi
    movq $buf, %rsi
    movq %r12, %rdx
    syscall
    jmp loop

exit:
    # _exit(n)
    movq %rax, %rdi
    movq $60, %rax
    syscall
    