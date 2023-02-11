#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>

#define NUMBER_OF_POINTS 10000000
#define H 0.001
#define FIRST_VALUE 0

typedef union rdtsc_time{
    unsigned long long rdtsc_time_64;
    struct rdtsc_time_32{
        int rdtsc_low_32, rdtsc_hight_32;
    }rdtsc_time_32;
} rdtsc_time;

double instructionIntrinsic_latency()
{
    int base = 3;
    int power = 256; //value of this variable must be divisible by 16 e.g. power % 16 == 0
    int power_copy = power;
    rdtsc_time firstCheck, secondCheck;
    asm volatile(
        "movl %5, %%r8d\n"
        "movl %4, %%r9d\n"
        "rdtsc\n"
        "movl %%eax, %2\n"
        "movl %%edx, %3\n"
        "cycle_start%=:\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r10d\n"
        "subl $16, %%r8d\n"
        "cmpl $0, %%r8d\n"
        "jg cycle_start%=\n"
        "rdtsc\n"
        "movl %%eax, %0\n"
        "movl %%edx, %1\n"
        :"=r"(secondCheck.rdtsc_time_32.rdtsc_low_32), "=r"(secondCheck.rdtsc_time_32.rdtsc_hight_32), 
        "=r"(firstCheck.rdtsc_time_32.rdtsc_low_32), "=r"(firstCheck.rdtsc_time_32.rdtsc_hight_32)
        :"r"(base), "r"(power)
        :"eax", "edx"
    );
    return ((double)(secondCheck.rdtsc_time_64 - firstCheck.rdtsc_time_64)) / power_copy;
}

double instructionIntrincic_throughput()
{
    int base = 3;
    int power = 256; //value of this variable must be divisible by 16 e.g. power % 16 == 0
    int power_copy = power;
    rdtsc_time firstCheck, secondCheck;
    asm volatile(
        "movl %5, %%r8d\n"
        "rdtsc\n"
        "movl %%eax, %2\n"
        "movl %%edx, %3\n"
        "movl %4, %%r9d\n"
        "cycle_start%=:\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r11d\n"
        "imull %%r9d, %%r12d\n"
        "imull %%r9d, %%r13d\n"
        "imull %%r9d, %%r14d\n"
        "imull %%r9d, %%r15d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r11d\n"
        "imull %%r9d, %%r12d\n"
        "imull %%r9d, %%r13d\n"
        "imull %%r9d, %%r14d\n"
        "imull %%r9d, %%r15d\n"
        "imull %%r9d, %%r10d\n"
        "imull %%r9d, %%r11d\n"
        "imull %%r9d, %%r12d\n"
        "imull %%r9d, %%r13d\n"
        "subl $16, %%r8d\n"
        "cmpl $0, %%r8d\n"
        "jg cycle_start%=\n"
        "rdtsc\n"
        "movl %%eax, %0\n"
        "movl %%edx, %1\n"
        :"=r"(secondCheck.rdtsc_time_32.rdtsc_low_32), "=r"(secondCheck.rdtsc_time_32.rdtsc_hight_32), 
        "=r"(firstCheck.rdtsc_time_32.rdtsc_low_32), "=r"(firstCheck.rdtsc_time_32.rdtsc_hight_32)
        :"r"(base), "r"(power)
        :"eax", "edx"
    );
    return ((double)(secondCheck.rdtsc_time_64 - firstCheck.rdtsc_time_64)) / power_copy;
}

double getFuncValue(double point)
{
    return sin(FIRST_VALUE + point * H);
}

double procWarmUp()
{
    //this function count first derivative of sin(x) with the second order with M = {0, 1, 2}
    double* derivative_array = (double*)calloc(NUMBER_OF_POINTS, sizeof(double));
    for(long long i = 0; i < NUMBER_OF_POINTS; ++i)
    {
        derivative_array[i] = (getFuncValue(i) * (-3) + getFuncValue(i + 1) * 4 + getFuncValue(i + 2) * (-1)) / (2 * H);
    }
    return derivative_array[NUMBER_OF_POINTS - 1];
}

int main()
{
    fprintf(stdout, "Start warming up...\n");
    double volatile workStuff = procWarmUp();
    fprintf(stdout, "Result of warm up: %lf\n", workStuff);
    fprintf(stdout, "Starting tests...\n");
    double time_1 = instructionIntrinsic_latency();
    double time_2 = instructionIntrincic_throughput();
    fprintf(stdout, "Instruction latency: %lf tacts, throughput: %lf tacts\n", time_1, time_2);
    return 0;
}