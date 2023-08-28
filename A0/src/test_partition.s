 .data

# Test data.  If you change the number of words, make sure to also
# change the argument passed in a2 to partition.

array1:
        .word 100
        .word 100
        .word 100000
        .word 255
        .word 0
        .word 123
        .word 4
        .word 50
        
array2:
        .word 1
        .word 2
        .word 3
        .word 4
        .word 5
        .word 6
        .word 7
        .word 8
        .word 9
        .word 10
        .word 11
        .word 12
        .word 13
        .word 14
        .word 15
        .word 16

array3:
	.word 100
        .word 2
        .word 3
        .word 4
        .word 5
        .word 6
        .word 7
        .word 8
        .word 9
        .word 10
        .word 11
        .word 12
        .word 13
        .word 14
        .word 15
        .word 16

.text
test:
        la a0, array3      # address of array
        addi a1, zero, 0  # start element
        addi a2, zero, 16  # number of elements
        jal ra, partition # call partition
        jal zero, end     # jump to end

.include "partition.s"

.text
end:
