#include "residual.h"

/*
 * x: [N][16][32][32]
 * f(x): [N][16][32][32]
 * 
 * output: [N][16][32][32]
 */

void residual_forward_0(data_t out[N][FIL_0][H_0][W_0],
                        data_t in[N][FIL_0][H_0][W_0],
                        data_t fx[N][FIL_0][H_0][W_0])
{
    int n, c, h, w;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL_0; c++) {
            for (h=0; h < H_0; h++) {
                for (w=0; w < W_0; w++) {
#pragma HLS UNROLL
                    out[n][c][h][w] = in[n][c][h][w] + fx[n][c][h][w];
                }
            }
        }
    }
}


void residual_forward_0_1(data_t out[N][FIL_1][H_1][W_1],
                        data_t in[N][FIL_0][H_0][W_0],
                        data_t fx[N][FIL_1][H_1][W_1],
                        const data_t conv_weight[FIL_1][FIL_0][WEI_H][WEI_W],
                        const data_t conv_bias[FIL_1],
                        const data_t bn_mean[FIL_1],
                        const data_t bn_var[FIL_1],
                        const data_t bn_gamma[FIL_1],
                        const data_t bn_beta[FIL_1])
{
    data_t in_padded[N][FIL_1][H_1][W_1];
    conv_forward_block_1_1(in_padded, in, conv_weight, conv_bias);
    bn_forward_1(in_padded, in_padded, bn_mean, bn_var, bn_gamma, bn_beta);

    int n, c, h, w;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL_1; c++) {
            for (h=0; h < H_1; h++) {
                for (w=0; w < W_1; w++) {
#pragma HLS UNROLL
                    out[n][c][h][w] = in_padded[n][c][h][w] + fx[n][c][h][w];
                }
            }
        }
    }
}


/*
 * x: [N][32][16][16]
 * f(x): [N][32][16][16]
 * 
 * output: [N][32][16][16]
 */
void residual_forward_1(data_t out[N][FIL_1][H_1][W_1],
                        data_t in[N][FIL_1][H_1][W_1],
                        data_t fx[N][FIL_1][H_1][W_1])
{
    int n, c, h, w;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL_1; c++) {
            for (h=0; h < H_1; h++) {
                for (w=0; w < W_1; w++) {
#pragma HLS UNROLL
                    out[n][c][h][w] = in[n][c][h][w] + fx[n][c][h][w];
                }
            }
        }
    }
}


void residual_forward_1_1(data_t out[N][FIL_2][H_2][W_2],
                        data_t in[N][FIL_1][H_1][W_1],
                        data_t fx[N][FIL_2][H_2][W_2],
                        const data_t conv_weight[FIL_2][FIL_1][WEI_H][WEI_W],
                        const data_t conv_bias[FIL_2],
                        const data_t bn_mean[FIL_2],
                        const data_t bn_var[FIL_2],
                        const data_t bn_gamma[FIL_2],
                        const data_t bn_beta[FIL_2])
{
    data_t in_padded[N][FIL_2][H_2][W_2];
    conv_forward_block_2_1(in_padded, in, conv_weight, conv_bias);
    bn_forward_2(in_padded, in_padded, bn_mean, bn_var, bn_gamma, bn_beta);
    
    int n, c, h, w;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL_2; c++) {
            for (h=0; h < H_2; h++) {
                for (w=0; w < W_2; w++) {
#pragma HLS UNROLL
                    out[n][c][h][w] = in_padded[n][c][h][w] + fx[n][c][h][w];
                }
            }
        }
    }
}

/*
 * x: [N][64][8][8]
 * f(x): [N][64][8][8]
 * 
 * output: [N][64][8][8]
 */
void residual_forward_2(data_t out[N][FIL_2][H_2][W_2],
                        data_t in[N][FIL_2][H_2][W_2],
                        data_t fx[N][FIL_2][H_2][W_2])
{
    int n, c, h, w;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL_2; c++) {
            for (h=0; h < H_2; h++) {
                for (w=0; w < W_2; w++) {
#pragma HLS UNROLL
                    out[n][c][h][w] = in[n][c][h][w] + fx[n][c][h][w];
                }
            }
        }
    }
}
