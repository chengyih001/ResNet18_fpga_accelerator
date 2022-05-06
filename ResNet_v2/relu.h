#pragma once
#include "params.h"

// x: [N][16][32][32]; output: [N][16][32][32]
void relu_forward_0(data_t out[N][FIL_0][H_0][W_0], data_t in[N][FIL_0][H_0][W_0]);

// x: [N][32][16][16]; output: [N][32][16][16]
void relu_forward_1(data_t out[N][FIL_1][H_1][W_1], data_t in[N][FIL_1][H_1][W_1]);

// x: [N][64][8][8]; output: [N][64][8][8]
void relu_forward_2(data_t out[N][FIL_2][H_2][W_2], data_t in[N][FIL_2][H_2][W_2]);