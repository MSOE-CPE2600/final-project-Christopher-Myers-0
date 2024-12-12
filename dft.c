/*******************************************************************************
Filename: dft.c
Name: Christopher Myers
Date: 12/10/2024
Course: CPE 2600-121
*******************************************************************************/

#include <stdio.h>
#include <complex.h>
#include <math.h>

int dft(double complex *x_in, int x_size, double complex *x_out) {

    for(int k = 0; k < x_size; k++)
    {
        x_out[k] = 0;
        for(int n = 0; n < x_size; n++)
        {
            x_out[k] += x_in[n]*cexp(-I*2.0*M_PI*((float)k/x_size)*n);
        }
        //printf("x_out[%d] = %f + I*%f\n", k, creal(x_out[k]), cimag(x_out[k]));

    }

    return 0;
}

int idft(double complex *x_k, int x_size, double complex *x_n) {

    for(int n = 0; n < x_size; n++)
    {
        x_n[n] = 0;
        for(int k = 0; k < x_size; k++)
        {
            x_n[n] += x_k[k]*cexp(I*2.0*M_PI*((float)k/x_size)*n);
        }
        x_n[n] = x_n[n] / x_size;
        //printf("x_out[%d] = %f + I*%f\n", k, creal(x_out[k]), cimag(x_out[k]));

    }

    return 0;
}