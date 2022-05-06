#pragma once
#include "params.h"

// x: [N][3][32][32]; output: [N][16][32][32]
void conv_forward_0(data_t out[N][FIL_0][H_0][W_0], data_t in[N][C][H_0][W_0], const data_t weight[FIL_0][C][WEI_H][WEI_W], const data_t bias[FIL_0]);

// x: [N][16][32][32]; output: [N][16][32][32]
void conv_forward_block_1(data_t out[N][FIL_0][H_0][W_0], data_t in[N][FIL_0][H_0][W_0], const data_t weight[FIL_0][FIL_0][WEI_H][WEI_W], const data_t bias[FIL_0]);

// x: [N][16][32][32]; output: [N][32][16][16]
void conv_forward_block_1_1(data_t out[N][FIL_1][H_1][W_1], data_t in[N][FIL_0][H_0][W_0], const data_t weight[FIL_1][FIL_0][WEI_H][WEI_W], const data_t bias[FIL_1]);

// x: [N][32][16][16]; output: [N][32][16][16]
void conv_forward_block_2(data_t out[N][FIL_1][H_1][W_1], data_t in[N][FIL_1][H_1][W_1], const data_t weight[FIL_1][FIL_1][WEI_H][WEI_W], const data_t bias[FIL_1]);

// x: [N][32][16][16]; output: [N][64][8][8]
void conv_forward_block_2_1(data_t out[N][FIL_2][H_2][W_2], data_t in[N][FIL_1][H_1][W_1], const data_t weight[FIL_2][FIL_1][WEI_H][WEI_W], const data_t bias[FIL_2]);

// x: [N][64][8][8]; output: [N][64][8][8]
void conv_forward_block_3(data_t out[N][FIL_2][H_2][W_2], data_t in[N][FIL_2][H_2][W_2], const data_t weight[FIL_2][FIL_2][WEI_H][WEI_W], const data_t bias[FIL_2]);
