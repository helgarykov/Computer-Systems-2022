        .text
partition:
        # array in a0
        # p in a1
        # r in a2

        # ADD CODE HERE



int partition(int *array, int p, int r) {
    
    int pivot = array[p];
    int i=p-1;
    int j=r;
    
    while (1) {
        
        do { j--; } while (pivot < array[j]);
        do { i++; } while (array[i] < pivot);
        if (i < j) {
            int tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
        } else {
            return j+1;
        }
    }
}

#---Help---#
# a0 = array address (in the end a0 will contain Return value)
# a1 = pivot index (int)
# a2 = last index of the segment within the array (r & j)
# a3 = pivot element val

partition:

    # line 13

    slli a3, a1, 2                          # a3 = p * 4 byte
    
    add a3, a0, a3                          # a3 = *array[p]

    lw a3, 0(a3)                            # a3 = array[p] (int val)
    
    # line 14

    addi a1, a1, -1                         # a1 = (i) = p - 1 (a1 is overwritten at this step)

partition_loop:
    
    addi a2, a2, -1                         # a2 (j) = j--;

    # find the val of array[j]
    slli t0, a2, 2                          # t0 = j * 4 byte
    add t0, t0, a0                          # t0 = *array[j]
    lw t1, 0(t0)                            # t1 = array[j] (int val)
    
    # if pivot < array[j] jump up to the while-loop
    blt a3, t1, partition_loop

partition_while_i_loop:

    addi a1, a1, 1                          # a1 (i) = i++;

    # find the val of array[j]
    slli t2, a1, 2                          # t2 = i * 4 byte
    add t2, t2, a0                          # t2 = *array[i]
    lw t3, 0(t2)                            # t3 = array[i] (int val)
    
    # if (array[i] < pivot) jump up to the while-loop
    blt t3, a3, partition_while_i_loop

    # if (i >= j) jump to Return
    bge a1, a2, partition_return
    
    sw t1, 0(t2)                            # *array[i] = array[j] (val)

    sw t3, 0(t0)                            # *array[j] = array[i] (val)
    
    jal zero, partition_loop

partition_return:

    addi a0, a2, 1                          # a0 = j + 1
    jalr zero, ra, 0                        # Jump to caller function
    

    


    
