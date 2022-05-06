#pragma once
#include <math.h>
#include "params.h"

// x: [N][16][32][32]; output: [N][16][32][32]
void bn_forward_0(data_t out[N][FIL_0][H_0][W_0], data_t in[N][FIL_0][H_0][W_0], const data_t running_mean[FIL_0], const data_t running_var[FIL_0], const data_t gamma[FIL_0], const data_t beta[FIL_0], data_t eps=0.00001);

// x: [N][32][16][16]; output: [N][32][16][16]
void bn_forward_1(data_t out[N][FIL_1][H_1][W_1], data_t in[N][FIL_1][H_1][W_1], const data_t running_mean[FIL_1], const data_t running_var[FIL_1], const data_t gamma[FIL_1], const data_t beta[FIL_1], data_t eps=0.00001);

// x: [N][64][8][8]; output: [N][64][8][8]
void bn_forward_2(data_t out[N][FIL_2][H_2][W_2], data_t in[N][FIL_2][H_2][W_2], const data_t running_mean[FIL_2], const data_t running_var[FIL_2], const data_t gamma[FIL_2], const data_t beta[FIL_2], data_t eps=0.00001);
