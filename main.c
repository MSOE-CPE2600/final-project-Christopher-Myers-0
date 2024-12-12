/*******************************************************************************
Filename: main.c
Name: Christopher Myers
Date: 12/10/2024
Course: CPE 2600-121
gcc -o main.out main.c dft.c -g -lm
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <string.h>
#include "dft.h"

//void generateData(double complex *x_in, int x_size);

void main(void) {

    FILE *fileIn;
    FILE *fileOut;
    char line[500];
    int width, height;

    // Open input and output file
    fileIn = fopen("barbara.pgm", "rb");
    fileOut = fopen("myFileOut.pgm", "wb");
    if(fileIn == NULL || fileOut == NULL) {
        fputs("Error opening file\n", stderr);
        exit(1);
    }

    // Read through header strings
    for(int i = 0; i < 4; i++) {
        fgets(line, 500, fileIn);
        if(i == 1) {
            fprintf(fileOut, "#Edited by Christopher Myers\n");
            printf("#Edited by Christopher Myers\n");
        } else if(i == 2) { // Read in width and height data
            fprintf(fileOut, "%s", line);
            printf("%s", line);
            int newlineIndex = strcspn(line, "\n");
            if(newlineIndex != strlen(line)) {
                line[newlineIndex] = '\0';
            }
            char *widthPtr = strtok(line, " ");
            char *heightPtr = strtok(NULL, " ");
            if(widthPtr != NULL)
            {
                width = strtol(widthPtr, NULL, 10);
            } else
            {
                perror("failed to tokenize width");
                exit(1);
            }
            if(widthPtr != NULL)
            {
                height = strtol(heightPtr, NULL, 10);
            } else
            {
                perror("failed to tokenize heigth");
                exit(1);
            }
        } else {
            fprintf(fileOut, "%s", line);
            printf("%s", line);
        }
    }

    unsigned char imgArray[width][height];
    double complex *row_k = malloc(sizeof(double complex) * width);
    double complex *row_n = malloc(sizeof(double complex) * width);
    //sleep(1);
    // Produce output
    for(int r = 0; r < height; r++) {
        for(int c = 0; c < width; c++) {
            imgArray[r][c] = (unsigned char)fgetc(fileIn);
            row_n[c] = imgArray[r][c];
        }

        dft(row_n, width, row_k);

        for(int k = 0; k < width; k++)
        {   
            if(k < width*20/100.0)
            {
                row_k[k] = 0;
            }
        }

        idft(row_k, width, row_n);

        for(int n = 0; n < width; n++)
        {
            //printf("%d\n", (unsigned char)creal(row_n[n])%256);
            fputc((unsigned char)creal(row_n[n])%256, fileOut);
        }
    }
    fclose(fileIn);

    for(int r = 0; r < height; r++)
    {
        
    }
    // for(int n = 0; n < x_size; n++)
    // {
    //     // printf("x_in[%d] = %f + I*%f : x_out[%d] = %f + I*%f : x_n[%d] = %f + I*%f\n", n, creal(x_in[n]), cimag(x_in[n]), n, creal(x_out[n]), cimag(x_out[n]), n, creal(x_n[n]), cimag(x_n[n]));
    //     // printf("%f, %f\n", (float)n/x_size*7000, creal(x_out[n]));
    // }

    free(row_k);
    free(row_n);
    fclose(fileOut);
}

// void generateData(double complex *x_in, int x_size) {
//     double sampleRate = 2*M_PI*7000;
//     for(int n = 0; n < x_size; n++)
//     {
//         double t = n/sampleRate;
//         x_in[n] = cos(2*M_PI*3000*t) + cos(2*M_PI*650*t) + 2*cos(2*M_PI*67*t) + cos(2*M_PI*3100*t);
//     }
// }