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
} input_t;

void print_binary(int bin_number, int size);
int get_adjustment(int bin_number);
float reconstructor(union_float x);
void ieee754_print(union_float x);

union_float sum(input_t var1, input_t var2, int op);

int main()
{
    input_t var1;
    input_t var2;

    union_float result;

    printf("Enter first number: ");
    scanf("%f", &var1.number_input.f);
    printf("Enter second number: ");
    scanf("%f", &var2.number_input.f);

    ieee754_print(var1.number_input);
    ieee754_print(var2.number_input);


    result = sum(var1, var2, 1);
    printf("\n Sum is: ");
    ieee754_print(result);

    result = sum(var1, var2, 0);
    printf("\n Subtraction is: ");
    ieee754_print(result);
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

// get_adjustment returns the amount of necessary shifts to normalize the sum of two binary numbers
int get_adjustment(int bin_number)
{
    if (bin_number & 0x8000000)
        return 4;
    if (bin_number & 0x4000000)
        return 3;
    if (bin_number & 0x2000000)
        return 2;
    if (bin_number & 0x1000000)
        return 1;
    if (bin_number & 0x0800000)
        return 0;
    if (bin_number & 0x0400000)
        return -1;
    if (bin_number & 0x0200000)
        return -2;
    if (bin_number & 0x0100000)
        return -3;
    if (bin_number & 0x0080000)
        return -4;
    else
        return -23;
}

// reconstructor manually forms the float version of a IEE754 binary
float reconstructor(union_float x)
{
    return pow(-1,(x.field.signal)) * ( 1.0 + x.field.mantissa / pow(2,23)) * pow(2,(x.field.power - 127));
}

// ieee754_print prints relevant data about a IEE754 binary
void ieee754_print(union_float x)
{
    printf("\n\n");
    printf("%d ", x.field.signal);
    print_binary(x.field.power, 8);
    printf(" ");
    print_binary(x.field.mantissa, 23);
    printf("\n");
    printf("Number reconstructed: %f \n", reconstructor(x));
}

union_float sum(input_t var1, input_t var2, int op)
{
    int shift_value;
    int sum_mantissa;//teste
    int adjustment = 0;
    unsigned int mantissa_aux_1, mantissa_aux_2;
    int exp_aux, signal_aux;

    union_float sum_result;

    var1.exp = var1.number_input.field.power - 127;
    var2.exp = var2.number_input.field.power - 127;

    if (var1.exp >= var2.exp)
    {
        mantissa_aux_1 = var1.number_input.field.mantissa + 0x800000;
        mantissa_aux_2 = var2.number_input.field.mantissa + 0x800000;
        exp_aux = var1.exp;
        shift_value = var1.exp - var2.exp;
        signal_aux = var1.number_input.field.signal;
    }
    else
    {
        mantissa_aux_1 = var2.number_input.field.mantissa + 0x800000;
        mantissa_aux_2 = var1.number_input.field.mantissa + 0x800000;
        exp_aux = var2.exp;
        shift_value = var2.exp - var1.exp;
        signal_aux = var2.number_input.field.signal;
    }
    mantissa_aux_2 >>= shift_value;


    if(
        (op == 1 && var1.number_input.field.signal != var2.number_input.field.signal) ||
        (op == 0 && var1.number_input.field.signal == var2.number_input.field.signal)
    )
    {
        mantissa_aux_2 = ~(mantissa_aux_2) + 1;
    }

    sum_mantissa = mantissa_aux_2 + mantissa_aux_1;  
    adjustment = get_adjustment(sum_mantissa);
    sum_mantissa >>= adjustment;

    sum_result.field.power = exp_aux + adjustment + 127;
    sum_result.field.signal = signal_aux;
    sum_result.field.mantissa = sum_mantissa & 0x7fffff;

    return sum_result;
}