#include "memory.h"
#include "assembly.h"
#include "simulate.h"
#include <stdio.h>

/*
[0]         = zero
[1]         = ra
[2]         = sp
[3]         = gp
[4]         = tp
[5..7]      = t0 - t2
[8..9]      = s0 - s1
[10..17]    = a0 - a7
[18..27]    = s2 - s11
[28..31]    = t3 - t6
*/
int registers[32];


int interpreter(struct memory *mem, struct assembly *as, int *addr, FILE *log_file) {
    // Getting the instruction at the address
    int instruction = memory_rd_w(mem, *addr);

    // Precutting the instruction encoding into fields, so that the code becomes clean.
    int opcode  = instruction & 0x7f;
    int rd  = (instruction >> 7) & 0x1f;
    int func3  = (instruction >> 12) & 0x7;
    int rs1  = registers[(instruction >> 15) & 0x1f];
    int rs2  = registers[(instruction >> 20) & 0x1f];
    int shamt  = (instruction >> 20) & 0x1f;
    int func7  = (instruction >> 25) & 0x7f;
    const char* insn_a = assembly_get(as, *addr);

    if(log_file != NULL){
        fprintf(log_file, "Instruction: %s\n", insn_a);
        fprintf(log_file,"Instruction %d:\n", instruction);
        fprintf(log_file,"opcode = 0x%x\n", opcode);
        fprintf(log_file,"rd: %d\n", rd);
        fprintf(log_file,"rs1: %d\n", rs1);
        fprintf(log_file,"rs2: %d\n", rs2);
        fprintf(log_file,"func3: %d\n", func3);
        fprintf(log_file,"func7: %d\n", func7);
        fprintf(log_file,"\n");
        fprintf(log_file, "Instruction: %s\n", insn_a);
        for (int i = 0; i < 32; i++) fprintf(log_file,"register[%d] %d\n", i, registers[i]);
        fprintf(log_file,"\n");
    }  

    switch (opcode) { // opcode
        case 0x37: // LUI
            registers[rd] = (instruction & 0xfffff000);
            break;
        
        case 0x17: // AUIPC
            registers[rd] = (instruction & 0xfffff000) + *addr;
            break;

        case 0x6f: // JAL
            registers[rd] = *addr + 4;
            int imm20 = ((instruction & 0x80000000) >> 11)
                | ((instruction & 0x7fe00000) >> 20)
                | ((instruction & 0x00100000) >> 9)
                | ((instruction & 0x000ff000));
            if (instruction & 0x80000000) imm20 |= 0xffe00000;
            *addr = *addr + imm20 - 4;
            break;

        case 0x67: // JALR
            registers[rd] = *addr + 4;
            *addr =  rs1 + (instruction >> 20) - 4;
            break;

        case 0x63: // 
        
            switch (func3) {
                case 0x0: // BEQ
                    if (rs1 == rs2) {
                        int imm12 = ((instruction & 0x00000080) << 4)
                            | ((instruction & 0x00000f00) >> 7)
                            | ((instruction & 0x7e000000) >> 20)
                            | ((instruction & 0x80000000) >> 19);
                        if( imm12 & 0x1000 ) imm12 |= 0xffffe000;
                        *addr += imm12 - 4;
                    }
                    break;
                
                case 0x1: // BNE
                    if (rs1 != rs2) {
                        int imm12 = ((instruction & 0x00000080) << 4)
                            | ((instruction & 0x00000f00) >> 7)
                            | ((instruction & 0x7e000000) >> 20)
                            | ((instruction & 0x80000000) >> 19);
                        if( imm12 & 0x1000 ) imm12 |= 0xffffe000;
                        *addr += imm12 - 4;
                    }
                    break;

                case 0x4: // BLT
                    if (rs1 < rs2) {
                        int imm12 = ((instruction & 0x00000080) << 4)
                            | ((instruction & 0x00000f00) >> 7)
                            | ((instruction & 0x7e000000) >> 20)
                            | ((instruction & 0x80000000) >> 19);
                        if( imm12 & 0x1000 ) imm12 |= 0xffffe000;
                        *addr += imm12 - 4;
                    }
                    break;

                case 0x5: // BGE
                    if (rs1 >= rs2) {
                        int imm12 = ((instruction & 0x00000080) << 4)
                            | ((instruction & 0x00000f00) >> 7)
                            | ((instruction & 0x7e000000) >> 20)
                            | ((instruction & 0x80000000) >> 19);
                        if( imm12 & 0x1000 ) imm12 |= 0xffffe000;
                        *addr += imm12 - 4;
                    }
                    break;

                case 0x6: // BLTU
                    if ((unsigned int) rs1 < (unsigned int) rs2) {
                        int imm12 = ((instruction & 0x00000080) << 4)
                            | ((instruction & 0x00000f00) >> 7)
                            | ((instruction & 0x7e000000) >> 20)
                            | ((instruction & 0x80000000) >> 19);
                        if( imm12 & 0x1000 ) imm12 |= 0xffffe000;
                        *addr += imm12 - 4;
                    }
                    break;

                case 0x7: // BGEU
                    if ((unsigned int) rs1 >= (unsigned int) rs2) {
                        int imm12 = ((instruction & 0x00000080) << 4)
                            | ((instruction & 0x00000f00) >> 7)
                            | ((instruction & 0x7e000000) >> 20)
                            | ((instruction & 0x80000000) >> 19);
                        if( imm12 & 0x1000 ) imm12 |= 0xffffe000;
                        *addr += imm12 - 4;
                    }
                    break;
            }

            break;

        case 0x3: // 

            switch (func3) {
                case 0x0: // LB 
                {
                    int imm12 = (instruction >> 20);
                    if ( imm12 & 0x800 ) imm12 |= 0xfffff000;
                    registers[rd] = memory_rd_b(mem, (rs1 + imm12));
                    break;
                }
                
                case 0x1: // LH
                {
                    int imm12 = (instruction >> 20);
                    if ( imm12 & 0x800 ) imm12 |= 0xfffff000;
                    registers[rd] = memory_rd_h(mem, (rs1 + imm12));
                    break;
                }

                case 0x2: // LW
                {
                    int imm12 = (instruction >> 20);
                    if ( imm12 & 0x800 ) imm12 |= 0xfffff000;
                    registers[rd] = memory_rd_w(mem, (rs1 + imm12));
                    break;
                }

                case 0x4: // LBU
                {
                    int imm12 = (instruction >> 20);
                    if ( imm12 & 0x800 ) imm12 |= 0xfffff000;
                    registers[rd] = (unsigned int) memory_rd_b(mem, (rs1 + imm12));
                    break;
                }

                case 0x5: // LHU
                {
                    int imm12 = (instruction >> 20);
                    if ( imm12 & 0x800 ) imm12 |= 0xfffff000;
                    registers[rd] = (unsigned int) memory_rd_h(mem, (rs1 + imm12));
                    break;
                }
            }

            break;
        
        case 0x23: // 

            switch (func3) {
                case 0x0: // SB
                {
                    int imm12 = (((instruction >> 20) & 0xfe0) | ((instruction >> 7) & 0x1f));
                    if ( imm12 & 0x800 ) imm12 |= 0xfffff000;
                    memory_wr_b(mem, (rs1 + imm12), rs2); 
                    break;
                }
                
                case 0x1: // SH
                {
                    int imm12 = (((instruction >> 20) & 0xfe0) | ((instruction >> 7) & 0x1f));
                    if ( imm12 & 0x800 ) imm12 |= 0xfffff000;
                    memory_wr_h(mem, (rs1 + imm12), rs2); 
                    break;
                }

                case 0x2: // SW
                {
                    int imm12 = (((instruction >> 20) & 0xfe0) | ((instruction >> 7) & 0x1f));
                    if ( imm12 & 0x800 ) imm12 |= 0xfffff000;
                    memory_wr_w(mem, (rs1 + imm12), rs2); 
                    break;
                }
            }

            break;

        case 0x13: // 

            switch (func3) {
                case 0x0: // ADDI
                {
                    int immaddi = (instruction >> 20);
                    if( immaddi & 0x800 ) immaddi |= 0xfffff000;
                    registers[rd] = rs1 + immaddi;
                    break;
                }
                case 0x1: // SLLI
                    registers[rd] = rs1 << shamt;
                    break;
                
                case 0x2: // SLTI 
                    registers[rd] = (rs1 < (instruction >> 20)) ? 1 : 0; 
                    break;

                case 0x3: // SLTIU
                    registers[rd] = ((unsigned int) rs1 < (unsigned int) (instruction >> 20)) ? 1 : 0;
                    break; 

                case 0x4: // XORI
                    registers[rd] = rs1 ^ (instruction >> 20);
                    break;

                case 0x5: // 

                    switch (func7) {
                        case 0x0: //SRLI
                            registers[rd] = ((unsigned int) rs1) >> shamt;
                            break;
                        
                        case 0x20: // SRAI
                            registers[rd] = rs1 >> shamt;
                            /*
                            Small comment:
                            It seems that arithmetic shifts happen, when the integer is signed?
                            Otherwise it uses logical shifts: Look it up.
                            */ 
                            break;
                    }

                    break;

                case 0x6: // ORI
                    registers[rd] = rs1 | (instruction >> 20);
                    break;
                
                case 0x7: // ANDI
                    registers[rd] = rs1 & (instruction >> 20);
                    break;
            }

            break;

        case 0x33: // 

            switch (func3) {
                case 0x0: //

                    switch (func7) {
                        case 0x0: // ADD
                            registers[rd] = rs1 + rs2;
                            break;

                        case 0x20: // SUB
                            registers[rd] = rs1 - rs2;
                            break;

                        case 0x1: // MUL
                            registers[rd] = (int)(((long)rs1 * (long)rs2) & 0x00000000ffffffff);
                             /*
                            Small comment:
                            MUL returns a XLEN-bit * XLEN-bit multiplication.
                            Lower bits in the destination register.
                            No signed/unsigned variants.
                            */ 
                            break;
                    }

                    break;
                
                case 0x1: //

                    switch (func7) {
                        case 0x0: // SLL
                            registers[rd] = rs1 << rs2;
                            break;

                        case 0x1: // MULH
                            registers[rd] = (int)((((long)rs1 * (long)rs2) & 0xffffffff00000000) >> 32); 
                            /*
                             Multiplies 2 signed operands rs1 and rs2 and stores the upper 32 bits in rd.
                            */
                            break;
                    }

                    break;

                case 0x2: //

                    switch (func7) {
                        case 0x0: // SLT
                            registers[rd] = (rs1 < rs2) ? 1 : 0;
                            break;

                        case 0x1: // MULHSU
                            registers[rd] = (int)((((long)rs1 * (unsigned long)rs2) & 0xffffffff00000000) << 32); 
                            /*
                            Multiplies signed operand rs1 and unsigned r operands2 and stores the upper 32 bits in rd.
                            */
                            break;
                    }

                    break;

                case 0x3: //

                    switch (func7) {
                        case 0x0: // SLTU
                            registers[rd] = (rs1 < rs2) ? 1 : 0;
                            break; 

                        case 0x1: // MULHU
                            registers[rd] = (unsigned int)((((unsigned long) rs1 * (unsigned long) rs2) & 0xffffffff00000000) >> 32);  
                            /*
                            Multiplies 2 unsigned operands rs1 and rs2 and stores the upper 32 bits in rd.
                            Why not int rs1 * int rs2?
                            NB: returns 2xXLEN-bit product.
                            */
                            break;
                    }

                    break;

                case 0x4: //

                    switch (func7) {
                        case 0x0: // XOR
                            registers[rd] = rs1 ^ rs2;
                            break;

                        case 0x1: // DIV
                            registers[rd] = rs1 / rs2;
                            break;
                    }

                    break;
                case 0x5: //

                    switch (func7) {
                        case 0x0: // SRL
                            registers[rd] = (unsigned int)rs1 >> rs2;
                            break;

                        case 0x20: // SRA
                            registers[rd] = rs1 >> rs2;
                            /*
                            Shift right arithmetic: for signed ints.
                            */
                            break;

                        case 0x1: // DIVU
                            registers[rd] = (unsigned int)rs1 / (unsigned int)rs2;
                            break;
                    }

                    break;

                case 0x6: //

                    switch (func7) {
                        case 0x0: // OR
                            registers[rd] = rs1 | rs2;
                            break;

                        case 0x1: // REM
                            registers[rd] = (rs1 % rs2);
                            break;
                    }

                    break;
                
                case 0x7: //

                    switch (func7) {
                        case 0x0: // AND
                            registers[rd] = rs1 & rs2;
                            break;

                        case 0x1: // REMU
                            registers[rd] = (unsigned int)rs1 % (unsigned int)rs2;
                            break;
                    }

                    break;
            }

            break;
        
        case 0x73: // ECALL

            switch (registers[17]) {
                case 3:
                case 93:
                    return -1;
                    break;

                case 1:
                    registers[17] = getchar();
                    break;
                
                case 2:
                    putchar(registers[16]);
                    break;
                default:
                    break;
            }
            break;
    }
    *addr += 4;
    return 0;
}

long int simulate(struct memory *mem, struct assembly *as, int start_addr, FILE *log_file) {
    int program_counter = start_addr;
    long int counter = 0;
    while (interpreter(mem, as, &program_counter, log_file) == 0) {
        registers[0] = 0;
        if(log_file != NULL){
            fprintf(log_file, "Nr: %ld\n", counter+1);
            fprintf(log_file, "%d\n", program_counter);
        }
        counter++;
    }
    return counter;
}
