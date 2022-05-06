#include <hls_stream.h>
#include <hls_vector.h>
#include <math.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
//#include "assert.h"

#define BURST_BUFF_SIZE 256;


typedef float d_type;


#define N 5
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
#define CH_SIZE 8


extern "C"

// input: [N][in_fil][in_h][in_w]; output: [N][out_fil][out_h][out_w]
// layer 0 (conv): param_1 = FIL_X, param_2 = C, param_3 = WEI_H, param_4 = WEI_W
// layer 1 (bn): param_1 = FIL_X, param_2 = FIL_X, param_2 = FIL_X, param_2 = FIL_X
// layer 2 (relu)
void vadd(d_type* in, d_type* out, d_type* weight, d_type* bias,
                int param_1, int param_2, int param_3, int param_4,
                int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w,
                int stride, int pad,
                int layer, int residual_save);
