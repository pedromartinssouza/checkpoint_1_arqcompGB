/* Project: BRTL Floating Point Calculator
 * Version: 0.1
 * Authors: Leonardo Dutra, Pedro Souza and Rafael Lawisch
 * Date:  02/11/21
 * */

#include <stdio.h>
#include <math.h>

typedef union {
    float f;
    struct {
        unsigned int mantissa : 23;
        unsigned int power : 8;
        unsigned int signal : 1;
    } field;
} union_float;

typedef struct input {
    union_float number_input;
    int exp;
    int mantissa_aux;
} input_t;

void print_binary(int bin_number, int size);
int calc_adjustment(int bin_number1, int bin_number2);
float reconstructor(union_float x);
void ieee754_printer(union_float x);

int main()
{
    input_t var1;
    input_t var2;

    int shift_value;
    int sum_mantissa;//teste
    int adjustment;
    union_float sum_result;

    printf("Enter first number: ");
    scanf("%f", &var1.number_input.f);
    printf("Enter second number: ");
    scanf("%f", &var2.number_input.f);

    ieee754_printer(var1.number_input);
    ieee754_printer(var2.number_input);

    var1.exp = var1.number_input.field.power - 127;
    var2.exp = var2.number_input.field.power - 127;

    var1.mantissa_aux = var1.number_input.field.mantissa + 0x800000;
    var2.mantissa_aux = var2.number_input.field.mantissa + 0x800000;

    if (var1.exp != var2.exp)
    {
        if(var1.exp>var2.exp)
        {
            shift_value = var1.exp - var2.exp;
            var2.mantissa_aux = var2.mantissa_aux >> shift_value;
            adjustment = calc_adjustment(var1.mantissa_aux, var2.mantissa_aux);

            sum_mantissa = (var2.mantissa_aux >> adjustment) + (var1.mantissa_aux >> adjustment);

            sum_result.field.power = var1.number_input.field.power + adjustment;
            sum_result.field.signal = var1.number_input.field.signal;
        } else
        {
            shift_value = var2.exp - var1.exp;
            var1.mantissa_aux = var1.mantissa_aux >> shift_value;
            adjustment = calc_adjustment(var1.mantissa_aux, var2.mantissa_aux);

            sum_mantissa = (var1.mantissa_aux >> adjustment) + (var2.mantissa_aux >> adjustment);

            sum_result.field.power = var2.number_input.field.power + adjustment;
            sum_result.field.signal = var2.number_input.field.signal;
        }

        sum_result.field.mantissa = sum_mantissa;
        ieee754_printer(sum_result);
    } else
    {
        adjustment = calc_adjustment(var1.mantissa_aux, var2.mantissa_aux);
        sum_mantissa = (var1.mantissa_aux >> adjustment) + (var2.mantissa_aux >> adjustment);
        sum_result.field.power = var1.number_input.field.power + adjustment;
        sum_result.field.signal = var1.number_input.field.signal;
        sum_result.field.mantissa = sum_mantissa;

        ieee754_printer(sum_result);
    }

    return 0;
}

// print_binary prints binary numbers
void print_binary(int bin_number, int size)
{
    int k;
    for (size--; size >= 0; size--)
    {
        k = bin_number >> size;
        if(k & 1)
            printf("1");
        else
            printf("0");
    }
}

// calc_adjustment finds out what is the necessary shift value to normalize a number
// the logic consists of examining how many 1's there are after running an AND operation against the two numbers
// to be added
int calc_adjustment(int bin_number1, int bin_number2)
{
    int bin_number = bin_number1 & bin_number2;
    int aux = 0;
    while ((bin_number << aux) & 0x800000) // while first number of shifted bin_number is 1, stay in loop (bin 1 followed by 23 zeros)
    {
        aux++;
    }
    return aux;
}

// reconstructor manually forms the float version of a IEE754 binary
float reconstructor(union_float x)
{
    return pow(-1,(x.field.signal)) * ( 1.0 + x.field.mantissa / pow(2,23)) * pow(2,(x.field.power - 127));
}

// ieee754_printer prints relevant data about a IEE754 binary
void ieee754_printer(union_float x)
{
    printf("\n\n");
    printf("%d ", x.field.signal);
    print_binary(x.field.power, 8);
    printf(" ");
    print_binary(x.field.mantissa, 23);
    printf("\n");
    printf("Number reconstructed: %f \n", reconstructor(x));
}