// COMP1521 22T3 Assignment 2: mipsc -- a MIPS simulator
// starting point code v1.0 - 24/10/22
//
// Author: Daniel Farnham (z5115421@unsw.edu.au) 
//
//
// Written 20/11/22 


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

// ADD ANY ADDITIONAL #include HERE

#define MAX_LINE_LENGTH 256
#define INSTRUCTIONS_GROW 64

struct instructions_to_print {
        char* instruction_type;
        int32_t value_s; 
        int32_t valuet; 
        int32_t value_d; 
        int32_t value_Imm; 
        int32_t value_in_reg;
        int32_t value_in_a0;
        int32_t value_HI;
        int32_t value_LO;
    };

typedef struct instructions_to_print Struct;
// ADD ANY ADDITIONAL #defines HERE

void execute_instructions(uint32_t n_instructions,
                          uint32_t instructions[],
                          int trace_mode);
char *process_arguments(int argc, char *argv[], int *trace_mode);
uint32_t *read_instructions(char *filename, uint32_t *n_instructions_p);
uint32_t *instructions_realloc(uint32_t *instructions, uint32_t n_instructions);
void print_bits(uint64_t value, int how_many_bits);
uint64_t extract_bits(int high_bit, int low_bit, uint64_t value);
void allocateRegister(int *RegisterArray, int RegisterN, int val); 
Struct add (int *RegisterArray, int bit_pattern);
Struct sub (int *RegisterArray, int bit_pattern);
Struct slt (int *RegisterArray, int bit_pattern);
Struct mfhi (int *RegisterArray, int *HILORegisterArray, int bit_pattern);
Struct mflo (int *RegisterArray, int *HILORegisterArray, int bit_pattern);
Struct mult (int *RegisterArray, int *HILORegisterArray, int bit_pattern);
Struct divide (int *RegisterArray, int *HILORegisterArray, int bit_pattern);
Struct mul (int *RegisterArray, int bit_pattern);
Struct beq(int *RegisterArray, uint32_t *pc_ptr, int bit_pattern);
Struct bne(int *RegisterArray, uint32_t *pc_ptr, int bit_pattern);
Struct addi (int *RegisterArray, int bit_pattern);
Struct ori (int *RegisterArray, int bit_pattern);
Struct lui (int *RegisterArray, int bit_pattern);
Struct syscall (int *RegisterArray, int RegisterN);


// ADD ANY ADDITIONAL FUNCTION PROTOTYPES HERE

// YOU DO NOT NEED TO CHANGE MAIN
// but you can if you really want to
int main(int argc, char *argv[]) {
    int trace_mode;
    char *filename = process_arguments(argc, argv, &trace_mode);

    uint32_t n_instructions;
    uint32_t *instructions = read_instructions(filename, &n_instructions);

    execute_instructions(n_instructions, instructions, trace_mode);

    free(instructions);
    return 0;
}


// simulate execution of  instruction codes in  instructions array
// output from syscall instruction & any error messages are printed
//
// if trace_mode != 0:
//     information is printed about each instruction as it executed
//
// execution stops if it reaches the end of the array
void execute_instructions(uint32_t n_instructions,
                          uint32_t instructions[],
                          int trace_mode) {

    // things to clean up 
        // use a variable of result to be declared here. this allows all the printing to be done at the end of the function. 
        // might not require instructions & append_last_six_bits
        // check a.value in register
        // the the result.value_in_reg for add and mul. 
        // might need to change uint to int in some cases e.g. mul 
        // could add in a print command to the function inputs. This can then be a marker for all of the printing to be done within the function. 
        // syscall could be improved by doing a lot of the logic within the function.
        // in some of the function implementation i need to swap around t and s. e.g. addi so that t = s + I
        // need to deal with division by 0. 
        // get rid of print bits

    // This array will store the values of the registers. 
    int RegisterArray[32]; 
    for (int32_t i = 0; i < 32; i++) {
        RegisterArray[i] = 0; 
    }

    // This array will store the values for the HI LO registers. 
    int HILORegisterArray[2]; 
    for (int32_t i = 0; i < 2; i++) {
        HILORegisterArray[i] = 0;
    }

    for (uint32_t pc = 0; pc < n_instructions; pc++) {

        // use these appended bits to check the type of instruction. 
        uint32_t first_six_bits = extract_bits(31, 26, instructions[pc]);
        uint32_t last_six_bits = extract_bits(5, 0, instructions[pc]);
        
        // Determining result type. 
        if (trace_mode) {
            if (first_six_bits == 0x00000000) {
                // Syscall 
                if (last_six_bits == 0x0000000C) {
                    Struct result = syscall(RegisterArray, 2);

                    printf("%u: 0x%08X %s\n", pc, instructions[pc], result.instruction_type);
                    printf(">>> %s %d\n", result.instruction_type, result.value_in_reg);
                    
                    // Printing value stored in a0
                    if (result.value_in_reg == 1) {
                        printf("<<< ");
                        printf("%d\n", result.value_in_a0);
                    }
                    if (result.value_in_reg == 10) {
                        exit(0); 
                    }
                    if (result.value_in_reg == 11) {
                        printf("<<< ");
                        printf("%c\n", result.value_in_a0);
                    }
                    if (result.value_in_reg != 1 && result.value_in_reg != 10 && result.value_in_reg != 11) {
                        fprintf(stderr, "Unknown system call: %d\n", result.value_in_reg);
                        exit(1); 
                    }
                }
                // this double if statement might be checking the same thing. 
                if (last_six_bits == 0x00000020) {
                    Struct result = add(RegisterArray, instructions[pc]); 

                    printf("%u: 0x%08X %s  $%d, $%d, $%d\n", pc, instructions[pc], result.instruction_type, result.value_d, result.valuet, result.value_s);
                    printf(">>> $%d = %d\n", result.value_d, result.value_in_reg);
                }
                
                if (last_six_bits == 0x00000022) {
                    Struct result = sub(RegisterArray, instructions[pc]); 

                    printf("%u: 0x%08X %s  $%d, $%d, $%d\n", pc, instructions[pc], result.instruction_type, result.value_d, result.valuet, result.value_s);
                    printf(">>> $%d = %d\n", result.value_d, result.value_in_reg);
                }

                if (last_six_bits == 0x0000002A) {
                    Struct result = slt(RegisterArray, instructions[pc]);

                    printf("%u: 0x%08X %s  $%d, $%d, $%d\n", pc, instructions[pc], result.instruction_type, result.value_d, result.valuet, result.value_s);
                    printf(">>> $%d = %d\n", result.value_d, result.value_in_reg);
                }

                if (last_six_bits == 0x00000010) {
                    
                    Struct result = mfhi(RegisterArray, HILORegisterArray, instructions[pc]);
                    printf("%u: 0x%08X %s $%d\n", pc, instructions[pc], result.instruction_type, result.value_d); 
                    printf(">>> $%d = %d\n", result.value_d, result.value_in_reg);
         
                }

                if (last_six_bits == 0x00000012) {
                   
                    Struct result = mflo(RegisterArray, HILORegisterArray, instructions[pc]);
                    printf("%u: 0x%08X %s $%d\n", pc, instructions[pc], result.instruction_type, result.value_d); 
                    printf(">>> $%d = %d\n", result.value_d, result.value_in_reg);
           
                }

                if (last_six_bits == 0x00000018) {
                                  
                    Struct result = mult(RegisterArray, HILORegisterArray, instructions[pc]);

                    printf("%u: 0x%08X %s $%d, $%d\n", pc, instructions[pc], result.instruction_type, result.value_s, result.valuet);
                    printf(">>> HI = %d\n", result.value_HI);
                    printf(">>> LO = %d\n", result.value_LO);
                    
                }

                if (last_six_bits == 0x0000001A) {
                    
                    Struct result = divide(RegisterArray, HILORegisterArray, instructions[pc]);

                    printf("%u: 0x%08X %s  $%d, $%d\n", pc, instructions[pc], result.instruction_type, result.value_s, result.valuet);
                    printf(">>> HI = %d\n", result.value_HI);
                    printf(">>> LO = %d\n", result.value_LO);

                }                

            }

            if (first_six_bits == 0x0000001C) {
                Struct result = mul(RegisterArray, instructions[pc]); 

                printf("%u: 0x%08X %s  $%d, $%d, $%d\n", pc, instructions[pc], result.instruction_type, result.value_d, result.valuet, result.value_s);
                printf(">>> $%d = %d\n", result.value_d, result.value_in_reg); 
            }

            if (first_six_bits == 0x00000008) {
                Struct result = addi(RegisterArray, instructions[pc]);    
        
                printf("%u: 0x%08X %s $%d, $%d, %d\n", pc, instructions[pc], result.instruction_type, result.value_s, result.valuet, result.value_Imm);
                printf(">>> $%d = %d\n", result.value_s, result.value_in_reg); 
            }

            if (first_six_bits == 0x00000004) {
                int temp_pc = pc; 
                printf("%u: 0x%08X", pc, instructions[pc]);
                Struct result = beq(RegisterArray, &pc, instructions[pc]);
                
                printf(" %s  $%d, $%d, %d\n", result.instruction_type, result.valuet, result.value_s, result.value_Imm);
                
                if (temp_pc != pc) {
                    printf(">>> branch taken to PC = %d\n", pc + 1); 
                }
                else if (temp_pc == pc) {
                    printf(">>> branch not taken\n");
                }
            }

            if (first_six_bits == 0x00000005) {
                int temp_pc = pc; 
                printf("%u: 0x%08X", pc, instructions[pc]);
                Struct result = bne(RegisterArray, &pc, instructions[pc]);
                
                printf(" %s  $%d, $%d, %d\n", result.instruction_type, result.valuet, result.value_s, result.value_Imm);
                
                if (temp_pc != pc) {
                    printf(">>> branch taken to PC = %d\n", pc + 1); 
                }
                else if (temp_pc == pc) {
                    printf(">>> branch not taken\n");
                }
            }

            if (first_six_bits == 0x0000000D) {
       
                Struct result = ori(RegisterArray, instructions[pc]); 

                printf("%u: 0x%08X %s  $%d, $%d, %d\n", pc, instructions[pc], result.instruction_type, result.valuet, result.value_s, result.value_Imm);
                printf(">>> $%d = %d\n", result.valuet, result.value_in_reg); 
            }

            if (first_six_bits == 0x0000000F) {
                Struct result = lui(RegisterArray, instructions[pc]); 

                printf("%u: 0x%08X %s  $%d, %d\n", pc, instructions[pc], result.instruction_type, result.valuet, result.value_Imm);
                printf(">>> $%d = %d\n", result.valuet, result.value_in_reg); 
            }

        }

        if (trace_mode == 0) {
            if (first_six_bits == 0x00000000) {
                // Syscall 
                if (last_six_bits == 0x0000000C) {
                    Struct result = syscall(RegisterArray, 2);

                    if (result.value_in_reg == 1) {
                        printf("%d", result.value_in_a0);
                    }
                    if (result.value_in_reg == 10) {
                        exit(0); 
                    }
                    if (result.value_in_reg == 11) {
                        printf("%c", result.value_in_a0);
                    }
                    
                    if (result.value_in_reg != 1 && result.value_in_reg != 10 && result.value_in_reg != 11) {
                        fprintf(stderr, "Unknown system call: %d\n", result.value_in_reg);
                        exit(0); 
                    }
                }

                if (last_six_bits == 0x00000020) {
                    add(RegisterArray, instructions[pc]); 
                }

                if (last_six_bits == 0x00000022) {
                    sub(RegisterArray, instructions[pc]); 
                }

                if (last_six_bits == 0x0000002A) {
                    slt(RegisterArray, instructions[pc]);
                }

                

                if (last_six_bits == 0x00000010) {
                    mfhi(RegisterArray, HILORegisterArray, instructions[pc]);
                }

                if (last_six_bits == 0x00000012) {
                    mflo(RegisterArray, HILORegisterArray, instructions[pc]);
                }

                if (last_six_bits == 0x00000018) {     
                    mult(RegisterArray, HILORegisterArray, instructions[pc]);
                }

                if (last_six_bits == 0x0000001A) { 
                    divide(RegisterArray, HILORegisterArray, instructions[pc]);
                }
              
            }

            if (first_six_bits == 0x0000001C) {
                mul(RegisterArray, instructions[pc]); 
            }

            if (first_six_bits == 0x00000008) {
                addi(RegisterArray, instructions[pc]);    
            }

            if (first_six_bits == 0x00000004) {
                beq(RegisterArray, &pc, instructions[pc]);
            }

            if (first_six_bits == 0x00000005) {
                bne(RegisterArray, &pc, instructions[pc]); 
            } 
            
            if (first_six_bits == 0x0000000D) {
                ori(RegisterArray, instructions[pc]); 
            }

            if (first_six_bits == 0x0000000F) {
                lui(RegisterArray, instructions[pc]); 
            }

        }

        // Handling halting
        if (pc + 1 > n_instructions) {
            exit(0);
        }
        if (pc < 0) {
            fprintf(stderr, "Illegal branch to non-instruction: PC = ??");
            exit(1); 
        }
    }

    
}


// ADD YOUR FUNCTIONS HERE

// Extract bits s, t and d from bits.
// sums values stored in register t and s. 
// stores summed value in register d. 

Struct add (int *RegisterArray, int bit_pattern) { 

    // Conversions from binary to int. 
    uint16_t s = extract_bits(20, 16, bit_pattern); 
    uint16_t t = extract_bits(25, 21, bit_pattern); 
    uint16_t d = extract_bits(15, 11, bit_pattern);
    
    
    int16_t sum = RegisterArray[t] + RegisterArray[s]; 
    
    // Values to be returned for printing to stdout. 
    Struct a; 
    a.instruction_type = "add"; 
    a.valuet = t; 
    a.value_d = d;
    a.value_s = s; 
    a.value_in_reg = sum;

    // Sum allocated to register
    allocateRegister(RegisterArray, d, sum); 
     
    return a; 
}

// Extract bits s, t and d from bits.
// subtracts register s from register t. 
// stores subtracted value in register d.

Struct sub (int *RegisterArray, int bit_pattern) { 

     // Conversions from binary to int. 
    uint16_t s = extract_bits(20, 16, bit_pattern); 
    uint16_t t = extract_bits(25, 21, bit_pattern); 
    uint16_t d = extract_bits(15, 11, bit_pattern);
    
    int16_t minus = RegisterArray[t] - RegisterArray[s]; 

    // Values to be returned for printing to stdout.
    Struct a; 
    a.instruction_type = "sub"; 
    a.valuet = t; 
    a.value_d = d;
    a.value_s = s; 
    a.value_in_reg = minus;

    // Subtraction stored in register d. 
    allocateRegister(RegisterArray, d, minus); 
     
    return a; 
}

// Extract bits s, t and d from bits.
// checks if register s is > register t
// if true, slt is 1. else slt is 0. 

Struct slt (int *RegisterArray, int bit_pattern) {
    uint16_t s = extract_bits(20, 16, bit_pattern); 
    uint16_t t = extract_bits(25, 21, bit_pattern); 
    uint16_t d = extract_bits(15, 11, bit_pattern);
    uint16_t check_slt;

    if (RegisterArray[t] < RegisterArray[s]) {
        check_slt = 1; 
    }
    else {
        check_slt = 0; 
    }
    
    // Values to be returned for printing to stdout.
    Struct a; 
    a.instruction_type = "slt"; 
    a.valuet = t; 
    a.value_d = d;
    a.value_s = s; 
    a.value_in_reg = check_slt;

    allocateRegister(RegisterArray, d, check_slt); 
     
    return a; 
}

// Extract bits d
// stores the HI value in the registerArray

Struct mfhi (int *RegisterArray, int *HILORegisterArray, int bit_pattern) {
    
   
    int32_t d = extract_bits(15, 11, bit_pattern);

    // Values to be printed
    // HILORegisterArray[0] = HI
    Struct a; 
    a.instruction_type = "mfhi"; 
    a.value_d = d; 
    a.value_in_reg = HILORegisterArray[0]; 

    allocateRegister(RegisterArray, d, HILORegisterArray[0]); 

    return a;
}

// Extract bits d
// stores the LO value in the registerArray
Struct mflo (int *RegisterArray, int *HILORegisterArray, int bit_pattern) {
    

    int32_t d = extract_bits(15, 11, bit_pattern);

    // Values to be printed
    // HILORegisterArray[0] = LO
    Struct a; 
    a.instruction_type = "mflo"; 
    a.value_d = d; 
    a.value_in_reg = HILORegisterArray[1]; 

    allocateRegister(RegisterArray, d, HILORegisterArray[1]);

    return a;
}

// Multiplies two values s and t. 
// Resulting 64 bit signed int is stored as a HI and LO value.

Struct mult (int *RegisterArray, int *HILORegisterArray, int bit_pattern) {
    
    // registers t and s are extracted.
    int32_t t = extract_bits(20, 16, bit_pattern); 
    int32_t s = extract_bits(25, 21, bit_pattern); 

    int64_t extendedt = RegisterArray[t]; 
    int64_t extended_s = RegisterArray[s];
    
    int64_t big_multiply = extendedt * extended_s; 

    int32_t HI = extract_bits(63, 32, big_multiply); 
    int32_t LO = extract_bits(31, 0, big_multiply);

    // Resulting HI and LO values are stored special registers
    HILORegisterArray[0] = HI;
    HILORegisterArray[1] = LO;

    Struct a; 
    a.instruction_type = "mult"; 
    a.valuet = t; 
    a.value_s = s; 
    a.value_HI = HI;
    a.value_LO = LO; 
    
    return a;

}

// Divides two values s and t. 
// Resulting 64 bit signed int is stored as a HI and LO value.
// HI value is modulus
// LO value is quotient

Struct divide (int *RegisterArray, int *HILORegisterArray, int bit_pattern) {
    
    // registers t and s are extracted.
    int32_t t = extract_bits(20, 16, bit_pattern); 
    int32_t s = extract_bits(25, 21, bit_pattern); 
    int32_t HI; 
    int32_t LO; 

    // Handling division by 0. 
    if (RegisterArray[t] == 0) {
        HI = 0; 
        LO = 0; 
    }

    // Calculation of modulus and quotient
    else {
        HI = RegisterArray[s] % RegisterArray[t]; 
        LO = RegisterArray[s] / RegisterArray[t]; 
    }
    
    HILORegisterArray[0] = HI;
    HILORegisterArray[1] = LO;

    // Values to be printed
    Struct a; 
    a.instruction_type = "div"; 
    a.valuet = t; 
    a.value_s = s; 
    a.value_HI = HI;
    a.value_LO = LO; 
    
    return a;

}

// Multiplies two values s and t
// Stores result in register d. 

Struct mul (int *RegisterArray, int bit_pattern) { 

    // extracting registers
    uint16_t s = extract_bits(20, 16, bit_pattern); 
    uint16_t t = extract_bits(25, 21, bit_pattern); 
    uint16_t d = extract_bits(15, 11, bit_pattern);

    int16_t mul = RegisterArray[t] * RegisterArray[s]; 
    
    // instructions to be printed
    Struct a; 
    a.instruction_type = "mul"; 
    a.valuet = t; 
    a.value_d = d;
    a.value_s = s; 
    a.value_in_reg = mul;

    allocateRegister(RegisterArray, d, mul); 
     
    return a; 
}

// Checks if the value stored in two registers s and t are equal. 
// If they are, instruction = current intstruction + Imm. 

Struct beq(int *RegisterArray, uint32_t* pc_ptr, int bit_pattern) {

    // extracting bits 
    uint16_t s = extract_bits(20, 16, bit_pattern); 
    uint16_t t = extract_bits(25, 21, bit_pattern); 
    int16_t Imm = extract_bits(15, 0, bit_pattern); 
       
    Struct a; 
    a.instruction_type = "beq";
    a.value_s = s; 
    a.valuet = t; 
    a.value_Imm = Imm; 

    // Checks if values are equal, if true jumps to instruction defined by Imm. 
    if (RegisterArray[s] == RegisterArray[t]) {
        *pc_ptr += Imm - 1;
    }

    return a; 
}

// Checks if the value stored in two registers s and t are not equal. 
// If they are, instruction = current intstruction + Imm. 
Struct bne(int *RegisterArray, uint32_t* pc_ptr, int bit_pattern) {

    uint16_t s = extract_bits(20, 16, bit_pattern); 
    uint16_t t = extract_bits(25, 21, bit_pattern); 
    int16_t Imm = extract_bits(15, 0, bit_pattern); 
       
    Struct a; 
    a.instruction_type = "bne";
    a.value_s = s; 
    a.valuet = t; 
    a.value_Imm = Imm; 
 
    // Checks if values are not equal, if true jumps to instruction defined by Imm. 
    if (RegisterArray[s] != RegisterArray[t]) {
        *pc_ptr += Imm - 1;
    }

    return a; 
}

// Adds an Imm value to a value stored in a register.
Struct addi (int *RegisterArray, int bit_pattern) {
   
    // Extracting bits
    int16_t Imm = extract_bits(15, 0, bit_pattern); 
    uint16_t s = extract_bits(20, 16, bit_pattern); 
    uint16_t t = extract_bits(25, 21, bit_pattern); 

    // Summing value in register t with Imm 
    int16_t sum = RegisterArray[t] + Imm; 

    // Instructions to be printed
    Struct a; 
    a.instruction_type = "addi"; 
    a.valuet = t; 
    a.value_Imm = Imm; 
    a.value_s = s;
    a.value_in_reg = sum;

    allocateRegister(RegisterArray, s, sum); 

    return a;

}

// Ori instruction. Applies the or operator on a register with a given Imm. 
Struct ori (int *RegisterArray, int bit_pattern) {
   
    // Extracting bits from instruction
    int32_t Imm = extract_bits(15, 0, bit_pattern);
    int16_t I = extract_bits(15, 0, bit_pattern); 
    int32_t t = extract_bits(20, 16, bit_pattern); 
    int32_t s = extract_bits(25, 21, bit_pattern);

    // Appling or operator with register[s] and Imm. 
    int32_t or_operator = RegisterArray[s] | Imm; 

    // Instructions to print
    Struct a; 
    a.instruction_type = "ori"; 
    a.valuet = t; 
    a.value_s = s; 
    a.value_Imm = I; 
    a.value_in_reg = or_operator;

    allocateRegister(RegisterArray, t, or_operator); 

    return a;

}

// Shift an Imm 16 bits to the left. 
Struct lui (int *RegisterArray, int bit_pattern) {
   
    // Extracting bits from instruction
    int32_t Imm = extract_bits(15, 0, bit_pattern); 
    int32_t t = extract_bits(20, 16, bit_pattern); 

    // Shifting bits
    int32_t shift16 = Imm << 16; 

    // Instructions to prin
    Struct a; 
    a.instruction_type = "lui"; 
    a.valuet = t; 
    a.value_Imm = Imm; 
    a.value_in_reg = shift16;

    allocateRegister(RegisterArray, t, shift16); 

    return a;

}

// Calling syscall. Providing instructions to be printed. 
// Most of the syscall printing happens within the the execute_instructions function.
Struct syscall (int *RegisterArray, int RegisterN) {

    Struct a; 

    a.value_in_reg = RegisterArray[RegisterN]; 
    a.instruction_type = "syscall"; 
    a.value_in_a0 = RegisterArray[4]; 


    if (RegisterN != 2) {
        fprintf(stderr, "Wrong register input into function\n"); 
        exit(0); 
    }

    return a; 

}

// HELPER FUNCTIONS

// Function design has been based of the set_bit_range.c function authored in Bitwise Operations section of COMP1521 notes
// Notable changes is for the function to handle 64 bit numbers and the passing of the bit instruction directly into the function. 
uint64_t extract_bits(int high_bit, int low_bit, uint64_t value) {
    
    uint64_t mask = 1;
    int n_bits = 8 * sizeof mask; 
    
    assert(low_bit >= 0);
    assert(high_bit >= low_bit);
    assert(high_bit < n_bits);

    int mask_size = high_bit - low_bit + 1; 
    mask = mask << mask_size; 
    mask = mask - 1; 
    mask = mask << low_bit; 

    uint64_t extracted_bits = value & mask; 

    extracted_bits = extracted_bits >> low_bit; 

    return extracted_bits; 
}

// When called store a given value in a given position in the register array. 
void allocateRegister(int *RegisterArray, int RegisterN, int val) {
   
    RegisterArray[RegisterN] = val; 

    // This overrides any attempts to change register 0 to anything other than 0.
    if (RegisterArray[0] != 0) {
        RegisterArray[0] = 0; 
    }
    

}



// DO NOT CHANGE ANY CODE BELOW HERE


// check_arguments is given command-line arguments
// it sets *trace_mode to 0 if -r is specified
//         *trace_mode is set to 1 otherwise
// the filename specified in command-line arguments is returned
char *process_arguments(int argc, char *argv[], int *trace_mode) {
    if (
        argc < 2 ||
        argc > 3 ||
        (argc == 2 && strcmp(argv[1], "-r") == 0) ||
        (argc == 3 && strcmp(argv[1], "-r") != 0)
    ) {
        fprintf(stderr, "Usage: %s [-r] <file>\n", argv[0]);
        exit(1);
    }
    *trace_mode = (argc == 2);
    return argv[argc - 1];
}


// read hexadecimal numbers from filename one per line
// numbers are return in a malloc'ed array
// *n_instructions is set to size of the array
uint32_t *read_instructions(char *filename, uint32_t *n_instructions_p) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror(filename);
        exit(1);
    }

    uint32_t *instructions = NULL;
    uint32_t n_instructions = 0;
    char line[MAX_LINE_LENGTH + 1];
    while (fgets(line, sizeof line, f) != NULL) {

        // grow instructions array in steps of INSTRUCTIONS_GROW elements
        if (n_instructions % INSTRUCTIONS_GROW == 0) {
            instructions = instructions_realloc(instructions, n_instructions + INSTRUCTIONS_GROW);
        }

        char *endptr;
        instructions[n_instructions] = (uint32_t)strtoul(line, &endptr, 16);
        if (*endptr != '\n' && *endptr != '\r' && *endptr != '\0') {
            fprintf(stderr, "line %d: invalid hexadecimal number: %s",
                    n_instructions + 1, line);
            exit(1);
        }
        if (instructions[n_instructions] != strtoul(line, &endptr, 16)) {
            fprintf(stderr, "line %d: number too large: %s",
                    n_instructions + 1, line);
            exit(1);
        }
        n_instructions++;
    }
    fclose(f);
    *n_instructions_p = n_instructions;
    // shrink instructions array to correct size
    instructions = instructions_realloc(instructions, n_instructions);
    return instructions;
}


// instructions_realloc is wrapper for realloc
// it calls realloc to grow/shrink the instructions array
// to the specified size
// it exits if realloc fails
// otherwise it returns the new instructions array
uint32_t *instructions_realloc(uint32_t *instructions, uint32_t n_instructions) {
    instructions = realloc(instructions, n_instructions * sizeof *instructions);
    if (instructions == NULL) {
        fprintf(stderr, "out of memory");
        exit(1);
    }
    return instructions;
}
