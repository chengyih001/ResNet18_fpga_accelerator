#include "relu.h"

/*
 * x: [N][16][32][32]
 * 
 * output: [N][16][32][32]
 */
void relu_forward_0(data_t out[N][FIL_0][H_0][W_0],
                    data_t in[N][FIL_0][H_0][W_0])
{
    int n, c, h, w;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL_0; c++) {
            for (h=0; h < H_0; h++) {
                for (w=0; w < W_0; w++) {
#pragma HLS UNROLL
                    if (in[n][c][h][w] > 0) {
                        out[n][c][h][w] = in[n][c][h][w];
                    }
                    else {
                        out[n][c][h][w] = 0;
                    }
                }
            }
        }
    }
}

/*
 * x: [N][32][16][16]
 * 
 * output: [N][32][16][16]
 */
void relu_forward_1(data_t out[N][FIL_1][H_1][W_1],
                    data_t in[N][FIL_1][H_1][W_1])
{
    int n, c, h, w;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL_1; c++) {
            for (h=0; h < H_1; h++) {
                for (w=0; w < W_1; w++) {
#pragma HLS UNROLL
                    if (in[n][c][h][w] > 0) {
                        out[n][c][h][w] = in[n][c][h][w];
                    }
                    else {
                        out[n][c][h][w] = 0;
                    }
                }
            }
        }
    }
}

/*
 * x: [N][64][8][8]
 * 
 * output: [N][64][8][8]
 */
void relu_forward_2(data_t out[N][FIL_2][H_2][W_2],
                    data_t in[N][FIL_2][H_2][W_2])
{
    int n, c, h, w;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL_2; c++) {
            for (h=0; h < H_2; h++) {
                for (w=0; w < W_2; w++) {
#pragma HLS UNROLL
                    if (in[n][c][h][w] > 0) {
                        out[n][c][h][w] = in[n][c][h][w];
                    }
                    else {
                        out[n][c][h][w] = 0;
                    }
                }
            }
        }
    }
}
