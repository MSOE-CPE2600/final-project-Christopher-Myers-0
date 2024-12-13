/*******************************************************************************
Filename: row_filter_struct.h
Name: Christopher Myers
Date: 12/13/2024
Course: CPE 2600-121
*******************************************************************************/

#ifndef ROW_FILTER_STRUCT
#define ROW_FILTER_STRUCT

typedef struct {
    int width;
    int height;
    int chunkIndex;
    int NUM_THREADS;
    unsigned char *imgArray;
    unsigned char *imgArrayOut;
} row_filter_struct;

#endif