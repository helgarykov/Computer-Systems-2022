//
//  main.c
//  Decimal_To_Binary
//
//  Created by Helga on 18/11/2022.
//
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include<pthread/pthread.h>


int x = 0;
void* thread_result;

void* worker(void* thread_result) {
    for (int i = 0; i < 1000; i++) {
        x++;
    }
    printf("%d\n", x);
    return NULL;
}



//how to assemble digits into a number, adding each digit to the left
//4321
// 0 + 1*1 -> 1
// 1 + 2*10 ->21
//21 + 3*100 -> 321

//how to assemble digits into a number, adding each digit to the right (easier)
// 0*10 + 1 -> 1
// 1*10 + 2 -> 12
// 12*10 + 3 ->123


/* Convert from decimal number to binary number */
uint64_t decimal_to_binary(uint64_t decimal) {
    uint64_t binary = 0;
    uint64_t multiplier = 1;
    uint64_t bin_digit;
    uint64_t decimal_buffer = decimal;
    while(decimal != 0){
        bin_digit=decimal%2;
        binary = binary + bin_digit*multiplier;
        multiplier *= 10;
        decimal = decimal / 2;
    }
    printf("Decimal (%llu) in binary: %llu\n",decimal_buffer, binary); //that's the correct final output
    return binary;
}

/* Convert from decimal to binary number, using arrays */
int* decimal_to_binary_array(uint64_t decimal) {
    int* binary = NULL;
    int bin_digit;
    uint64_t decimal_buffer = decimal;
    
    // Determine the size of the array
    int counter = 0;
    while(decimal != 0)
    {
        decimal = decimal / 2;                      // divide by 2 because the result array contains a binary namber
        counter++;
    }
    // Allocate memory to the array containing a binary represent of the decimal
    binary = malloc((counter+1) * sizeof(int));
    binary[0] = counter+1;                          // set the value of cell 0 to array size
    decimal = decimal_buffer;
    
    // Fill in the array with bits
    for(int i = binary[0]-1; i > 0; i--)
    {
        bin_digit=decimal%2;
        binary[i] = bin_digit;
        decimal = decimal / 2;
    }
    printf("\nDecimal (%llu) and as Binary: ", decimal_buffer);
    for(int i=1; i < binary[0]; i++)
    {
        printf("%d", binary[i]);
    }
    printf("\n");
    return binary;
}


void print_meta_array(int* array){
    
    for (int i = 1; i < array[0]; i++){
        printf("%d", array[i]);
    }
    printf(" ");
}

/* Convert float (single precision 32 bit) to binary */
/* sign param: "+" if sign > 0 else "-" */
void float_to_binary_32(float number, int sign){
    uint64_t integral_10 = number;
    int* integral_2 = decimal_to_binary_array(integral_10);
    int counter_integral_bits = integral_2[0] - 1;
   

    // Compute Exp field
    int e = counter_integral_bits - 1;    // the total number of bits in integral - the 1st digit
    
    printf("Exp field [8 bit] as ");
    int* exp = decimal_to_binary_array (e + 127);
    
    // Compute integral part of mantissa
    int* integral_part_mantissa = malloc((integral_2[0]-1) * sizeof(int));
    integral_part_mantissa[0] = integral_2[0] - 1;
    for(int i = 1; i < integral_part_mantissa[0]; i++){
        integral_part_mantissa[i] = integral_2[i+1];
    }
    
    // Compute mantissa fractional part
    long double fractional_10 = number - integral_10;
    printf("Fractional as Decimal (%Lf) \n", fractional_10);
    int bits_in_fracitional = 23 - e;
    int mantissa_fractional [bits_in_fracitional];
    int counter_fractional_bits = 0;
    for (int i = 0; i < bits_in_fracitional; i++)
    {
        fractional_10 = fractional_10 * 2;
        if(fractional_10 > 1)
        {
            fractional_10 -= 1;
            mantissa_fractional[i] = 1;
        }
        else if(fractional_10 == 1)
        {
            mantissa_fractional[i] = 1;
            for (++i; i < bits_in_fracitional; ++i)
            {
                mantissa_fractional[i] = 0;
            }
            break;
        }
        else
        {
            mantissa_fractional[i] = 0;
        }
        counter_fractional_bits++;
    }
    int sign_bit;
    if(sign > 0) sign_bit = 0;
    else sign_bit = 1;
    
    printf("The final result: %d ", sign_bit);
    print_meta_array(exp);
    print_meta_array(integral_part_mantissa);
    for(int i = 0; i < bits_in_fracitional; ++i)
    {
        printf("%d",mantissa_fractional[i]);
    }
    printf("\n");
}


/* Convert float (single precision 64 bit) to binary */
/* sign param: "+" if sign > 0 else "-" */
void float_to_binary_64(double number, int sign){
    uint64_t integral_10 = number;
    int* integral_2 = decimal_to_binary_array(integral_10);
    int counter_integral_bits = integral_2[0] - 1;
   

    // Compute Exp field
    int e = counter_integral_bits - 1;    // the total number of bits in integral - the 1st digit
    printf("Exp field [11 bit] as");
    int* exp = decimal_to_binary_array (e + 1023);
    
    // Compute integral part of mantissa
    int* integral_part_mantissa = malloc((integral_2[0]-1) * sizeof(int));
    integral_part_mantissa[0] = integral_2[0] - 1;
    for(int i = 1; i < integral_part_mantissa[0]; i++){
        integral_part_mantissa[i] = integral_2[i+1];
    }
    
    // Compute mantissa fractional part
    long double fractional_10 = number - integral_10;
    printf("Fractional as Decimal (%Lf) \n", fractional_10);
    int bits_in_fracitional = 52 - e;
    int mantissa_fractional [bits_in_fracitional];
    int counter_fractional_bits = 0;
    for (int i = 0; i < bits_in_fracitional; i++)
    {
        fractional_10 = fractional_10 * 2;
        if(fractional_10 > 1)
        {
            fractional_10 -= 1;
            mantissa_fractional[i] = 1;
        }
        else if(fractional_10 == 1)
        {
            mantissa_fractional[i] = 1;
            for (++i; i < bits_in_fracitional; ++i)
            {
                mantissa_fractional[i] = 0;
            }
            break;
        }
        else
        {
            mantissa_fractional[i] = 0;
        }
        counter_fractional_bits++;
    }
    int sign_bit;
    if(sign > 0) sign_bit = 0;
    else sign_bit = 1;
    
    printf("The final result: %d ", sign_bit);
    print_meta_array(exp);
    print_meta_array(integral_part_mantissa);
    for(int i = 0; i < bits_in_fracitional; ++i)
    {
        printf("%d",mantissa_fractional[i]);
    }
    printf("\n");
}


int main(int argc, const char * argv[]) {
    
    //decimal_to_binary_array(38);
    //decimal_to_binary_array(1846);
    /*
    float_to_binary_32(3.141, -1);           //  1 10000000 100100 10000011000100101
    float_to_binary_32(65.25, 1);            //  0 10000101 000001 01000000000000000
    float_to_binary_32(1864.784567, 1);*/      //  0 10001001 110100 10001100100011011
    
    float_to_binary_64(1864.784567, 1);      //  0 10000001001 1101001000 110010001101100101100010000001101000010101
    
     
    
    
    // Logical Right Shift for signed positive (0 ... 127) and unsigned ints (0 ... 255) and
    // Arithmetic Right Shift for signed negative ints (-128 ... 0)
    // https://www.omnicalculator.com/math/twos-complement
    
    /*
    signed int x1 = 5;
    assert((x1 >> 1) == 2);     // ( >> 1) <=> (floor(x1/2))
    printf("5 after right shift with 1 = %i \n", x1);
    
    signed int x2 = -5;
    signed int x3 = x2 >> 1;     // ( >> 1) <=> (ceil(x1/2)))+
    assert((x2 >> 1) == -3);
    printf("-5 after right shift with 1 = %i \n", x3);
    unsigned int x4 = (unsigned int)-5;
    assert((x4 >> 1) == 0x7FFFFFFD);
    
    
    // Logical Left Shift == Arithmetic Left shift
    signed int y1 = 5;
    signed int y2 = y1 << 1;
    assert((y1 << 1) == 10);
    printf("5 after left shift with 1 = %d\n", y2);
    
    signed int y3 = -5;
    signed int y4 = y3 << 1; //  (<< 1) <=> (* 2)
    assert((y3 << 1) == -10);
    printf("-5 after left shift with 1 = %d\n", y4);
    
    
    
    
    
    pthread_t t;
    pthread_create(&t, NULL, worker, NULL);
    pthread_join(t, &thread_result);
    pthread_create(&t, NULL, worker, NULL);
    pthread_join(t, &thread_result);
    printf("%d\n", x);
     */
    
    
    return 0;       // a zero constant in C means FALSE, any non-zero constant mean TRUE
}
