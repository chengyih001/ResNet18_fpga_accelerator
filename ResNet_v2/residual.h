#pragma once
#include "params.h"
#include "bn.h"
#include "conv.h"

// x: [N][16][32][32] + f(x): [N][16][32][32] = output: [N][16][32][32]
void residual_forward_0(data_t out[N][FIL_0][H_0][W_0], data_t in[N][FIL_0][H_0][W_0], data_t fx[N][FIL_0][H_0][W_0]);

// x: [N][16][32][32] + f(x): [N][32][16][16] = output: [N][32][16][16]
void residual_forward_0_1(data_t out[N][FIL_1][H_1][W_1], data_t in[N][FIL_0][H_0][W_0], data_t fx[N][FIL_1][H_1][W_1],
                        const data_t conv_weight[FIL_1][FIL_0][WEI_H][WEI_W], const data_t conv_bias[FIL_1], const data_t bn_mean[FIL_1],
                        const data_t bn_var[FIL_1], const data_t bn_gamma[FIL_1], const data_t bn_beta[FIL_1]);

// x: [N][32][16][16] + f(x): [N][32][16][16] = output: [N][32][16][16]
void residual_forward_1(data_t out[N][FIL_1][H_1][W_1], data_t in[N][FIL_1][H_1][W_1], data_t fx[N][FIL_1][H_1][W_1]);

// x: [N][32][16][16] + f(x): [N][64][8][8] = output: [N][64][8][8]
void residual_forward_1_1(data_t out[N][FIL_2][H_2][W_2], data_t in[N][FIL_1][H_1][W_1], data_t fx[N][FIL_2][H_2][W_2],
                        const data_t conv_weight[FIL_2][FIL_1][WEI_H][WEI_W], const data_t conv_bias[FIL_2], const data_t bn_mean[FIL_2],
                        const data_t bn_var[FIL_2], const data_t bn_gamma[FIL_2], const data_t bn_beta[FIL_2]);

// x: [N][64][8][8] + f(x): [N][64][8][8] = output: [N][64][8][8]
void residual_forward_2(data_t out[N][FIL_2][H_2][W_2], data_t in[N][FIL_2][H_2][W_2], data_t fx[N][FIL_2][H_2][W_2]);