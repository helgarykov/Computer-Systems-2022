.include "partition.s"
# deez niggas be out to get me! #

# a0 = int *array
# a1 = int start
# a2 = int end

quicksort:
	sub t0, a2, a1				# t0 = end-start
	addi t1, x0, 2				# t1 = 2
	
	blt t0, t1, quicksort_return		# if (end-start < 2) return;
	
	addi sp, sp, -20			# allocates memory for 5 bytes in the stack
	sw ra, 0(sp)
	sw a0, 4(sp)
	sw a1, 8(sp)
	sw a2, 12(sp)
	
	jal ra, partition			# runs partition
	
	sw a0, 16(sp)				# save q for later use
	mv a2, a0				# a2 = q
	lw a0, 4(sp)				# a0 = *array
	lw a1, 8(sp)				# a1 = start
	
	jal ra, quicksort			# runs quicksort recursively the 1st time
	
	# loads parameters for function call
	lw a1, 16(sp)
	lw a2, 12(sp)
	
	jal ra, quicksort			# runs quicksort recursively the 2nd time
	
	lw ra, 0(sp)				# loads the function's original return address
	addi sp, sp, 20				# deallocates 5 bytes of memory in the stack

quicksort_return:
	jalr zero, ra, 0
	
