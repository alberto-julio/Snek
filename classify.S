.globl classify

.text
# =====================================
# COMMAND LINE ARGUMENTS
# =====================================
# Args:
#   a0 (int)        argc
#   a1 (char**)     argv
#   a1[1] (char*)   pointer to the filepath string of m0
#   a1[2] (char*)   pointer to the filepath string of m1
#   a1[3] (char*)   pointer to the filepath string of input matrix
#   a1[4] (char*)   pointer to the filepath string of output file
#   a2 (int)        silent mode, if this is 1, you should not print
#                   anything. Otherwise, you should print the
#                   classification and a newline.
# Returns:
#   a0 (int)        Classification
# Exceptions:
#   - If there are an incorrect number of command line args,
#     this function terminates the program with exit code 31
#   - If malloc fails, this function terminates the program with exit code 26
#
# Usage:
#   main.s <M0_PATH> <M1_PATH> <INPUT_PATH> <OUTPUT_PATH>

classify:
     #prologue:
    addi sp sp -52
    sw ra 0(sp)
    sw s0 4(sp)
    sw s1 8(sp)
    sw s2 12(sp)
    sw s3 16(sp)
    sw s4 20(sp)
    sw s5 24(sp)
    sw s6 28(sp)
    sw s7 32(sp)
    sw s8 36(sp)
    sw s9 40(sp)
    sw s10 44(sp)
    sw s11 48(sp)
 
    mv s0 a0 
    mv s1 a1
    mv s2 a2 

    addi t0 x0 5 #saves t register to stack
    bne a0 t0 length_error

    #body start

    # Read pretrained m0
    addi a0 x0 4
    jal ra malloc
    beq a0 x0 malloc_error
    addi s3 a0 0 #get the rows, add 4 and get the cols
    addi a0 x0 4 #store in s3 and s4
    jal ra malloc
    beq a0 x0 malloc_error
    addi s4 a0 0

    lw a0 4(s1) #get the first index of a1??
    addi a1 s3 0 #move rows and cols to s3 and s4
    addi a2 s4 0 

    jal ra read_matrix
    mv s5 a0

    # Read pretrained m1
    addi a0 x0 4
    jal ra malloc
    beq a0 x0 malloc_error
    addi s6 a0 0 #get the rows, add 4 and get the cols
    addi a0 x0 4 #store in s3 and s4
    jal ra malloc
    beq a0 x0 malloc_error
    addi s7 a0 0

    lw a0 8(s1) #get second index of a1??
    addi a1 s6 0 #move rows and cols to s6 and s7
    addi a2 s7 0 

    jal ra read_matrix
    mv s8 a0


    # Read input matrix
    addi a0 x0 4 
    jal malloc
    addi s9 a0 0 #get rows 
    beq a0 x0 malloc_error
    addi a0 x0 4 
    jal ra malloc 
    addi s10 a0 0 #get cols
    beq a0 x0 malloc_error

    lw a0 12(s1)
    addi a1 s9 0 #s9  saved for rows
    addi a2 s10 0 #s10  saved for columns
    jal ra read_matrix 
    mv s11 a0 #store matrix to s11
    
    # load values from row/col pointers 
    mv a0 s3
    lw s3 0(s3) # m0 rows
    jal free
    mv a0 s4
    lw s4 0(s4) # m0 cols
    jal free
    mv a0 s6
    lw s6 0(s6) # m1 rows
    jal free
    mv a0 s7
    lw s7 0(s7) # m1 cols
    jal free
    mv a0 s9
    lw s9 0(s9) # input rows
    jal free
    mv a0 s10
    lw s10 0(s10) # input cols
    jal free
    
    
    # Compute h = matmul(m0, input)
    # malloc for matmul
    mul a0 s3 s10
    li t0 4
    mul a0 a0 t0
    jal ra malloc
    beq a0 x0 malloc_error
    mv a6 a0 # malloc result
    mv s0 a0 # POINTER TO H ARRAY
    mv a0 s5 # m0 matrix
    mv a1 s3 # m0 rows
    mv a2 s4 # m0 cols
    mv a3 s11 # input matrix
    mv a4 s9 # input rows
    mv a5 s10 # input cols
    
    jal ra matmul


    # Compute h = relu(h)
    mv a0 s0
    mul a1 s3 s10
    jal relu
   
    # Compute o = matmul(m1, h)
    # malloc for matmul
    mul a0 s6 s10
    li t0 4
    mul a0 a0 t0
    jal ra malloc
    beq a0 x0 malloc_error
    mv s9 a0 # POINTER TO O ARRAY
    mv a6 a0
    mv a0 s8
    mv a1 s6
    mv a2 s7
    mv a3 s0
    mv a4 s3
    mv a5 s10
    jal ra matmul

    # Write output matrix o
    lw a0 16(s1)
    mv a1 s9
    mv a2 s6 # o rows
    mv a3 s10 # o cols
    jal ra write_matrix


    # Compute and return argmax(o)
    mv a0 s9
    mul a1 s6 s10
    jal ra argmax
    mv s10 a0


    # If enabled, print argmax(o) and newline
    beq s2 x0 print_argmax
    
    #body end
    # free registers
    
    addi a0 s5 0
    jal free

    addi a0 s8 0
    jal free
    addi a0 s0 0
    jal free
    addi a0 s9 0
    jal free

    addi a0 s11 0
    jal free
    
    mv a0 s10

    #epilogue
    lw ra 0(sp)
    lw s0 4(sp)
    lw s1 8(sp)
    lw s2 12(sp)
    lw s3 16(sp)
    lw s4 20(sp)
    lw s5 24(sp)
    lw s6 28(sp)
    lw s7 32(sp)
    lw s8 36(sp)
    lw s9 40(sp)
    lw s10 44(sp)
    lw s11 48(sp)
    addi sp sp 52

    jr ra

# free_regs:

# #     addi a0 s3 0
# #     jal free
# #     addi a0 s4 0
# #     jal free
#     addi a0 s5 0
#     jal free
# #     addi a0 s6 0
# #     jal free
# #     addi a0 s7 0
# #     jal free
#     addi a0 s8 0
#     jal free
# #     addi a0 s9 0
# #     jal free
# #     addi a0 s10 0
# #     jal free
#     addi a0 s11 0
#     jal free

malloc_error:
    li a0 26
    j exit

length_error:
    li a0 31
    j exit
    
print_argmax:
    mv a0 s10
    jal print_int
    li a0 '\n'
    jal print_char
    
    addi a0 s5 0
    jal free
   
    addi a0 s0 0
    jal free
    addi a0 s8 0
    jal free
    
    addi a0 s9 0
    jal free

    addi a0 s11 0
    jal free
    mv a0 s10

    #epilogue
    lw ra 0(sp)
    lw s0 4(sp)
    lw s1 8(sp)
    lw s2 12(sp)
    lw s3 16(sp)
    lw s4 20(sp)
    lw s5 24(sp)
    lw s6 28(sp)
    lw s7 32(sp)
    lw s8 36(sp)
    lw s9 40(sp)
    lw s10 44(sp)
    lw s11 48(sp)
    addi sp sp 52

    jr ra
    