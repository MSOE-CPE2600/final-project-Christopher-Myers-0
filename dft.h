/*******************************************************************************
Filename: dft.h
Name: Christopher Myers
Date: 12/10/2024
Course: CPE 2600-121
*******************************************************************************/

#include <stdio.h>
#include <complex.h>

int dft(double complex *x_in, int x_size, double complex *x_out);

int idft(double complex *x_k, int x_size, double complex *x_n);