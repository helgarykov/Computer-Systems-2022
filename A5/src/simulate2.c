#include "memory.h"
#include "assembly.h"
#include "simulate.h"
#include <stdio.h>
#include <math.h>

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
enum bitsize{byte, halfword, word, ubyte, uhalfword, uword};
enum ALU_signalActions {
    ALU_LUI = 0,
    B_BEQ = 1,
    B_BNE = 2,
    B_BLT = 3,
    B_BGE = 4,
    B_BLTU = 5,
    B_BGEU = 6,
    ALU_ADD = 7,
    ALU_SUB = 8,
    ALU_SLL = 9,
    ALU_SLT = 10,
    ALU_SLTU = 11,
    ALU_XOR = 12,
    ALU_SRL = 13,
    ALU_SRA = 14,
    ALU_OR = 15,
    ALU_AND = 16,
    ALU_MUL = 17,
    ALU_DIV = 18,
    ALU_REM = 19,
};

int getField(int insn, int end, int start) {
    int length = end-start+1;
    int mask = pow(2, length)-1;
    return mask & (insn >> start);
}

void setSignals(int opcode, int funct3, int* Branch, int* MemRead, int* MemToReg, int* ALUOp0, int* ALUOp1, int* MemWrite, int* ALUSrc, int* RegWrite, enum bitsize* size) {
    *ALUOp0 = 0; //not used, hmm...
    switch(opcode) {
        case 0x37: //LUI
            *RegWrite = 1;
            *ALUSrc = 1;
            break;
        
        case 0x17: //AUIPC
            *ALUSrc = 1;
            *RegWrite = 1;
            break;

        case 0x6f: //JAL
            *ALUSrc = 1;
            *RegWrite = 1;
            break;

        case 0x67: //JALR
            *Branch = 1;
            *RegWrite = 1;
            break;

        case 0x63: //B
            *Branch = 1;
            *ALUOp1 = 1;
            break;

        case 0x3: //L
            switch (funct3)
            {
            case 0x0:
                *size = byte;
                break;
            case 0x1:
                *size = halfword;
                break;
            case 0x2:
                *size = word;
                break;
            case 0x4:
                *size = ubyte;
                break;
            case 0x5:
                *size = uhalfword;
                break;
            }
            *MemRead = 1;
            *ALUSrc = 1;
            *MemToReg = 1;
            *RegWrite = 1;
            break;

        case 0x23: //S
            switch (funct3)
            {
            case 0x0:
                *size = byte;
                break;
            case 0x1:
                *size = halfword;
                break;
            case 0x2:
                *size = word;
                break;
            }
            *ALUSrc = 1;
            *MemWrite = 1;
            break;

        case 0x13: //I
            *ALUSrc = 1;
            *ALUOp1 = 1;
            *RegWrite = 1;
            break;
        
        case 0x33: //RM
            *ALUOp1 = 1;
            *RegWrite = 1;
            break;

        case 0x73: //ECALL - Nothing to set
            break;

        default:
            break;
    }
}

int get_imm(int insn, int opcode){
    int imm;
    unsigned int imm_0 = 0;
    unsigned int imm_10_to_1 = 0;
    unsigned int imm_11 = 0;
    unsigned int imm_19_to_12 = 0;
    unsigned int imm_20 = 0;
    unsigned int imm_12 = 0;
    unsigned int imm_10_to_5 = 0;
    unsigned int imm_4_to_1 = 0;
    unsigned int imm_11_to_5 = 0;
    unsigned int imm_4_to_0 = 0;
    unsigned int imm2 = 0;

    switch (opcode) {
        case 0x37: //LUI
            return getField(insn, 31, 12);
        
        case 0x17: //AUIPC
            return getField(insn, 31, 12);

        case 0x6f: //JAL
            imm_0 = 0;
            imm_10_to_1 = (insn & 0x7fe00000) >> 20;
            imm_11 = (insn & 0x00100000) >> 9;
            imm_19_to_12 = (insn & 0x000ff000);
            imm_20 = (insn & 0x80000000) >> 11;
            imm2 = imm_20 | imm_19_to_12 | imm_11 | imm_10_to_1 | imm_0;
            if ((imm2 >> 20) != 0){
                imm2 = imm2 | 0xfff00000; // add sign extension
            }
            return imm2;

        case 0x67: //JALR
            imm = getField(insn, 31, 20);
            if((imm >> 11) == 1){
                return (imm | 0xFFFFF000);
            }else{
                return imm;
            }

        case 0x63: //B-format
            imm_0 = 0;
            imm_11 = (getField(insn, 7, 7) << 11);
            imm_4_to_1 = (getField(insn, 11, 8) << 1);
            imm_10_to_5 = (getField(insn, 30, 25) << 5);
            imm_12 = (getField(insn, 31, 31) << 12);
            imm = (imm_12 | imm_11 | imm_10_to_5 | imm_4_to_1 | imm_0);
            if((imm_12 >> 12) == 1){
                return imm = (imm | 0xFFFFF000);
            }else{
                return imm;
            }

        case 0x3: //L-format
            return getField(insn, 31, 20);

        case 0x23: //S-format
            imm_11_to_5 = (getField(insn, 31, 25) << 5);
            imm_4_to_0 = getField(insn, 11, 7);
            imm = ((imm_11_to_5) | imm_4_to_0);
            if((imm >> 11) == 1){
                return imm = (imm | 0xFFFFF000);
            }
            return imm;

        case 0x13: //I-format
            imm = ((insn & 0xFFF00000) >> 20);
            if((imm >> 11) == 1){
                return (imm | 0xFFFFF000);
            }else{
                return imm;
            }

        default:
            return -1;
    }
    return -1;
}
int ALU_control(int opcode, int ALUOp1, int ALUOp0, int funct7, int funct3){
    int bit25 = getField(funct7, 0, 0);
    int bit30 = getField(funct7, 5, 5);
    if(ALUOp1 == 0 && ALUOp0 == 0){
        //Check if LUI
        if(opcode == 0x37){
            return ALU_LUI;
        } else{
            return ALU_ADD;
        }
    }
    else if(ALUOp1 == 0 && ALUOp0 == 1){
        return ALU_SUB;
    }
    else if (ALUOp1 == 1 && ALUOp0 == 0)
    {
        //Check if I-format coding
        if(opcode == 0x13){
                switch (funct3) {
                    case 0x0:
                        return ALU_ADD;

                    case 0x1:
                        return ALU_SLL;

                    case 0x2:
                        return ALU_SLT;

                    case 0x3:
                        return ALU_SLTU;

                    case 0x4:
                        return ALU_XOR;
                    
                    case 0x5:
                        if(bit30 == 0){
                            return ALU_SRL;
                        } else{
                            return ALU_SRA;
                        }

                    case 0x6:
                        return ALU_OR;

                    case 0x7:
                        return ALU_AND;

                    default:
                        return 0;
                }
        }
        //Check if B-format coding
        else if (opcode == 0x63) { // if branch check funct3
            switch (funct3) {
                case 0x0:
                    return B_BEQ;
                    break;

                case 0x1:
                    return B_BNE;
                    break;

                case 0x4:
                    return B_BLT;
                    break;

                case 0x5:
                    return B_BGE;
                    break;

                case 0x6:
                    return B_BLTU;
                    break;

                case 0x7:
                    return B_BGEU;
                    break;
            }
        }
        //Check if R-format og M-Format coding
        else if (opcode == 0x33) { // Check bit 25 or bit 30 to check which one
            if (bit25) { // if M check funct3
                switch (funct3) {
                    case 0x0:
                        return ALU_MUL;

                    case 0x1:
                        return ALU_MUL;

                    case 0x2:
                        return ALU_MUL;

                    case 0x3:
                        return ALU_MUL;

                    case 0x4:
                        return ALU_DIV;

                    case 0x5:
                        return ALU_DIV;

                    case 0x6:
                        return ALU_REM;

                    case 0x7:
                        return ALU_REM;
                }
            }
            else if (bit30){ // else if R with bit30 set check funct3
                switch (funct3) {
                    case 0x0:
                        return ALU_SUB;

                    case 0x5:
                        return ALU_SRA;
                }
            }
            else {  // else R with bit 30 not set check funct3
                switch (funct3) {
                    case 0x0:
                        return ALU_ADD;

                    case 0x1:
                        return ALU_SLL;

                    case 0x2:
                        return ALU_SLT;

                    case 0x3:
                        return ALU_SLTU;

                    case 0x4:
                        return ALU_XOR;
                    
                    case 0x5:
                        return ALU_SRL;

                    case 0x6:
                        return ALU_OR;

                    case 0x7:
                        return ALU_AND;

                    default:
                        return 0;
                }
            }
        }
    }
    return -1;
}
int ALU_Action(int input1, int input2, enum ALU_signalActions ALU_signalActions){
    switch (ALU_signalActions) {
        case ALU_LUI:
            return (input2 << 12) & 0xfffff000;

        case ALU_ADD:
            return input1 + input2;

        case ALU_SUB:
            return input1 - input2;

        case ALU_OR:
            return input1 | input2;
        
        case ALU_AND:
            return input1 & input2;

        case ALU_XOR:
            return input1 ^ input2;

        case ALU_SLL:
            return input1 << input2;

        case ALU_SLT:
            return input1 < input2 ? 1 : 0;
        
        case ALU_SRL:
            return (unsigned int)input1 >> input2;

        case ALU_SRA:
            return input1 >> input2;

        case ALU_SLTU:
            return (unsigned int)input1 < (unsigned int)input2 ? 1 : 0;

        case ALU_MUL:
            return input1 * input2;

        case ALU_DIV:
            return input1 / input2;
        
        case ALU_REM:
            return input1 % input2;

        case B_BEQ:
            return input1 - input2;
        
        case B_BNE:
            return !(input1 - input2);

        case B_BLT:
            return !(input1 < input2);

        case B_BGE:
            return !(input1 >= input2);

        case B_BLTU:
            return !((unsigned int) input1 < (unsigned int) input2);

        case B_BGEU:
            return !((unsigned int) input1 >= (unsigned int) input2);

        default:
            break;
    }
    return -1;
}


long int simulate(struct memory *mem, struct assembly *as, int start_addr, FILE *log_file) {
    int PC = start_addr;
    long int num_insn = 0;
    int looping = 0;
    fprintf(log_file, "Start of program\n\n");
    //Loop until Ecall breaks the loop
    while (1) {
        // fetch instruction
        int instruction = memory_rd_w(mem, PC);
        int opcode = getField(instruction, 6, 0);
        int rd = getField(instruction, 11, 7);
        int func3 = getField(instruction, 14, 12);
        int rs1 = getField(instruction, 19, 15);
        int rs2 = getField(instruction, 24, 20);
        int func7 = getField(instruction, 31, 25);
        const char* instruction_a = assembly_get(as, PC);

        //Logging fetched instructions and rigister
        if(log_file != NULL){
            fprintf(log_file, "Nr: %ld\n", num_insn);
            fprintf(log_file, "Instructions: %s\n", instruction_a);
            fprintf(log_file,"Instruction %d:\n", instruction);
            fprintf(log_file,"opcode = 0x%x\n", opcode);
            fprintf(log_file,"rd: %d\n", rd);
            fprintf(log_file,"rs1: x[%d]\n", rs1);
            fprintf(log_file,"rs2: x[%d]\n", rs2);
            fprintf(log_file,"func3: x[%d]\n", func3);
            fprintf(log_file,"func7: x[%d]\n", func7);
            fprintf(log_file, "Registers: ");
            for(int i = 0; i < 32; i++){
                fprintf(log_file, "x[%d] = %d, ", i, registers[i]);
                if(i%5 == 0 && i != 0){
                    fprintf(log_file, "\n");
                }
            }
            fprintf(log_file,"\n\n"); 
        }  
        
        if(opcode == 0x73){ //Ecall
            looping++;
            if((registers[17] == 3 || registers[17] == 93)){
                return num_insn;
            }
            else if(registers[17] == 1){
                char c = getchar();
                registers[17] = c;
            } else if(registers[17] == 2){
                putchar(registers[16]);
            }
            PC += 4;
            continue;
        }


        int Branch = 0;
        int MemRead = 0;
        int MemToReg = 0;
        int ALUOp0 = 0;
        int ALUOp1 = 0;
        int MemWrite = 0;
        int ALUSrc = 0;
        int RegWrite = 0;
        enum bitsize s = word;

        // decode instruction, set signals
        setSignals(opcode, func3, &Branch, &MemRead, &MemToReg, &ALUOp0, &ALUOp1, &MemWrite, &ALUSrc, &RegWrite, &s);

        // Fetch immediate from instruction
        int ImmGen = get_imm(instruction, opcode);

        // get ALU action depending on control signals and instruction
        int ALU_signalActions = ALU_control(opcode, ALUOp1, ALUOp0, func7, func3);

        // execute ALU
        int ALU_result;
        if (ALUSrc) {
            //Check if JAL
            if (opcode == 0x6f) {
                ALU_result = ALU_Action(PC, ImmGen, ALU_signalActions);
            }
            //JALR
            else {
                ALU_result = ALU_Action(registers[rs1], ImmGen, ALU_signalActions);
            }
        }
        //Everything else
        else {
            ALU_result = ALU_Action(registers[rs1], registers[rs2], ALU_signalActions);
        }

        // data memory
        int address = ALU_result;
        int write_data;

        // if MemToReg read from memory
        if (MemToReg) {
            switch (s)
            {
            case byte:
                write_data = memory_rd_b(mem, address);
                break;
            case halfword:
                write_data = memory_rd_h(mem, address);
                break;
            case word:
                write_data = memory_rd_w(mem, address);
                break;
            case ubyte:
                write_data = (unsigned int) memory_rd_b(mem, address);
                break;
            case uhalfword:
                write_data = (unsigned int) memory_rd_h(mem, address);
                break;
            case uword:
                write_data = (unsigned int) memory_rd_w(mem, address);
                break;
            }
        }
        else { // if MemToReg is not set write_data is ALU result, except if MemWrite = 1; then write_data is the value at registers[rs2]
            if(MemWrite){
                write_data = registers[rs2];
            }else{
                write_data = ALU_result;
            }
        }

        // if memwrite is set write result to memory
        if (MemWrite) {
            switch (s)
            {
            case byte:
            case ubyte:
                memory_wr_b(mem, address, write_data);
                break;
            case halfword:
            case uhalfword:
                memory_wr_h(mem, address, write_data);
                break;
            case word:
            case uword:
                memory_wr_w(mem, address, write_data);
                break;
            }
        }

        // if RegWrite write to register
        if (RegWrite) {
            if(opcode == 0x67){ //JALR
                registers[rd] = PC + 4;
            } else if(opcode == 0x6f){ //JAL
                registers[rd] = PC + 4;
            }
            else{
                registers[rd] = write_data;
                if (rd == 0) {
                    registers[rd] = 0;
                }
            }
            
        }
        //Harcoding x0
        registers[0] = 0;
        
        // Update PC
        if (opcode == 0x6f) { //JAL
            PC = write_data - 4;
        } else if(opcode == 0x67){ //JALR
            PC = write_data - 4;
        }
        if (Branch == 1){
            if(ALU_result == 0){
                PC += ImmGen;
            }else{
                PC += 4;
            }
        }
        else {
            PC += 4;
        }

        num_insn++;
        looping++;
    }
return num_insn;
}

