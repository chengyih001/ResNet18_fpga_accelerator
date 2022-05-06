#include "bn.h"

/*
 * x: [N][16][32][32]
 * running_mean: [16]
 * running_var: [16]
 * gamma: [16]
 * beta: [16]
 * 
 * output: [N][16][32][32]
 */
void bn_forward_0(data_t out[N][FIL_0][H_0][W_0],
                  data_t in[N][FIL_0][H_0][W_0],
                  const data_t running_mean[FIL_0],
                  const data_t running_var[FIL_0],
                  const data_t gamma[FIL_0],
                  const data_t beta[FIL_0],
                  data_t eps)
{
    int n, c, h, w;
    for (n=0; n < N; n++) {
        for (c=0; c < FIL_0; c++) {
            for (h=0; h < H_0; h++) {
                for (w=0; w < W_0; w++) {
                    // out[n][c][h][w] = gamma[c] * (in[n][c][h][w] - running_mean[c]) / hls::sqrt((running_var[c] + eps)) + beta[c];
                     out[n][c][h][w] = gamma[c] * (in[n][c][h][w] - running_mean[c]) / sqrt((running_var[c] + eps)) + beta[c];
//                    out[n][c][h][w] = gamma[c] * (in[n][c][h][w] - running_mean[c]) / (running_var[c] + eps) + beta[c];

                }
            }
        }
    }
}

/*
 * x: [N][32][16][16]
 * running_mean: [32]
 * running_var: [32]
 * gamma: [32]
 * beta: [32]
 * 
 * output: [N][32][16][16]
 */
void bn_forward_1(data_t out[N][FIL_1][H_1][W_1],
                  data_t in[N][FIL_1][H_1][W_1],
                  const data_t running_mean[FIL_1],
                  const data_t running_var[FIL_1],
                  const data_t gamma[FIL_1],
                  const data_t beta[FIL_1],
                  data_t eps)
{
    int n, c, h, w;
    for (n=0; n < N; n++) {
        for (c=0; c < FIL_1; c++) {
            for (h=0; h < H_1; h++) {
                for (w=0; w < W_1; w++) {
                    // out[n][c][h][w] = gamma[c] * (in[n][c][h][w] - running_mean[c]) / hls::sqrt((running_var[c] + eps)) + beta[c];
                     out[n][c][h][w] = gamma[c] * (in[n][c][h][w] - running_mean[c]) / sqrt((running_var[c] + eps)) + beta[c];
//                    out[n][c][h][w] = gamma[c] * (in[n][c][h][w] - running_mean[c]) / (running_var[c] + eps) + beta[c];

                }
            }
        }
    }
}

/*
 * x: [N][64][8][8]
 * running_mean: [64]
 * running_var: [64]
 * gamma: [64]
 * beta: [64]
 * 
 * output: [N][64][8][8]
 */
void bn_forward_2(data_t out[N][FIL_2][H_2][W_2],
                  data_t in[N][FIL_2][H_2][W_2],
                  const data_t running_mean[FIL_2],
                  const data_t running_var[FIL_2],
                  const data_t gamma[FIL_2],
                  const data_t beta[FIL_2],
                  data_t eps)
{
    int n, c, h, w;
    for (n=0; n < N; n++) {
        for (c=0; c < FIL_2; c++) {
            for (h=0; h < H_2; h++) {
                for (w=0; w < W_2; w++) {
                    // out[n][c][h][w] = gamma[c] * (in[n][c][h][w] - running_mean[c]) / hls::sqrt((running_var[c] + eps)) + beta[c];
                     out[n][c][h][w] = gamma[c] * (in[n][c][h][w] - running_mean[c]) / sqrt((running_var[c] + eps)) + beta[c];
//                    out[n][c][h][w] = gamma[c] * (in[n][c][h][w] - running_mean[c]) / (running_var[c] + eps) + beta[c];

                }
            }
        }
    }
}

