#include <stdio.h>
#include <string.h>
#include <math.h>

#define N 20
#define FIL_1 16
#define FIL_2 32
#define FIL_3 64
#define H_1 32
#define W_1 32
#define H_2 16
#define W_2 16
#define H_3 8
#define W_3 8

#define CONV_H 3
#define CONV_W 3

#define _ -1

#define CLASS 10

void data_scatter_4d(float* input, float* output, 
                    int param_1, int param_2, int param_3, int param_4);

void data_scatter_2d(float* input, float* output, 
                    int param_1, int param_2);          

void data_scatter_1d(float* input, float* output, 
                    int param_1, int offset);

void data_gather(float* input, float* output,
                    int data_num);

void RESNET18_accelerator(float* input,  float* output, float* weight, float* bias, 
                int param_1, int param_2, int param_3, int param_4,
                int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w,
                int stride, int pad,
                int layer);