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
        for (c=0; c < FIL_0; c++) {
            for (h=0; h < H_0; h++) {
                for (w=0; w < W_0; w++) {
                    out[n][c][h][w] = in[n][c][h][w] + fx[n][c][h][w];
                }
            }
        }
    }
}

/*
 * x: [N][16][32][32]
 * f(x): [N][32][16][16]
 * 
 * output: [N][32][16][16]
 */

// void residual_conv_block_1_1(data_t out[N][FIL_1][H_1][W_1],
//                     data_t in[N][FIL_0][H_0][W_0], 
//                     const data_t weight[FIL_1][FIL_0][WEI_H][WEI_W], 
//                     const data_t bias[FIL_1])
// {
//     int n, c, h, w, f, hh, ww;
//     // pad x
//     data_t in_pad[N][FIL_0][H_0_PAD][W_0_PAD];
//     for (n=0; n < N; n++) {
//         for (c=0; c < FIL_0; c++) {
//             for (h=0; h < H_0; h++) {
//                 for (w=0; w < W_0; w++) {
//                     in_pad[n][c][h+PAD_0][w+PAD_0] = in[n][c][h][w];
//                 }
//             }
//         }
//     }

//     for (n=0; n < N; n++) {
//         for (f=0; f < FIL_1; f++) {
//             for (h=0; h < H_1; h++) {
//                 for (w=0; w < W_1; w++) {
//                     out[n][f][h][w] = bias[f];
//                     for (c=0; c < FIL_0; c++) {
//                         for (hh=0; hh < WEI_H; hh++) {
//                             for (ww=0; ww < WEI_W; ww++) {
//                                 out[n][f][h][w] += weight[f][c][hh][ww] * in_pad[n][c][h*STR_1+hh][w*STR_1+ww];
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     }
// }

// void residual_bn_1(data_t out[N][FIL_1][H_1][W_1],
//                   data_t in[N][FIL_1][H_1][W_1],
//                   const data_t running_mean[FIL_1],
//                   const data_t running_var[FIL_1],
//                   const data_t gamma[FIL_1],
//                   const data_t beta[FIL_1],
//                   data_t eps)
// {
//     int n, c, h, w;
//     for (n=0; n < N; n++) {
//         for (c=0; c < FIL_1; c++) {
//             for (h=0; h < H_1; h++) {
//                 for (w=0; w < W_1; w++) {
//                     out[n][c][h][w] = gamma[c] * (in[n][c][h][w] - running_mean[c]) / sqrt((running_var[c] + eps)) + beta[c];
//                 }
//             }
//         }
//     }
// }

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
        for (c=0; c < FIL_1; c++) {
            for (h=0; h < H_1; h++) {
                for (w=0; w < W_1; w++) {
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
        for (c=0; c < FIL_1; c++) {
            for (h=0; h < H_1; h++) {
                for (w=0; w < W_1; w++) {
                    out[n][c][h][w] = in[n][c][h][w] + fx[n][c][h][w];
                }
            }
        }
    }
}

/*
 * x: [N][32][16][16]
 * f(x): [N][64][8][8]
 * 
 * output: [N][64][8][8]
 */

// void residual_conv_block_2_1(data_t out[N][FIL_2][H_2][W_2],
//                     data_t in[N][FIL_1][H_1][W_1], 
//                     const data_t weight[FIL_2][FIL_1][WEI_H][WEI_W], 
//                     const data_t bias[FIL_2])
// {
//     int n, c, h, w, f, hh, ww;
//     // pad x
//     data_t in_pad[N][FIL_1][H_1_PAD][W_1_PAD];
//     for (n=0; n < N; n++) {
//         for (c=0; c < FIL_1; c++) {
//             for (h=0; h < H_1; h++) {
//                 for (w=0; w < W_1; w++) {
//                     in_pad[n][c][h+PAD_0][w+PAD_0] = in[n][c][h][w];
//                 }
//             }
//         }
//     }

//     for (n=0; n < N; n++) {
//         for (f=0; f < FIL_2; f++) {
//             for (h=0; h < H_2; h++) {
//                 for (w=0; w < W_2; w++) {
//                     out[n][f][h][w] = bias[f];
//                     for (c=0; c < FIL_1; c++) {
//                         for (hh=0; hh < WEI_H; hh++) {
//                             for (ww=0; ww < WEI_W; ww++) {
//                                 out[n][f][h][w] += weight[f][c][hh][ww] * in_pad[n][c][h*STR_1+hh][w*STR_1+ww];
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     }
// }

// void reseidual_bn_2(data_t out[N][FIL_2][H_2][W_2],
//                   data_t in[N][FIL_2][H_2][W_2],
//                   const data_t running_mean[FIL_2],
//                   const data_t running_var[FIL_2],
//                   const data_t gamma[FIL_2],
//                   const data_t beta[FIL_2],
//                   data_t eps)
// {
//     int n, c, h, w;
//     for (n=0; n < N; n++) {
//         for (c=0; c < FIL_2; c++) {
//             for (h=0; h < H_2; h++) {
//                 for (w=0; w < W_2; w++) {
//                     out[n][c][h][w] = gamma[c] * (in[n][c][h][w] - running_mean[c]) / sqrt((running_var[c] + eps)) + beta[c];
//                 }
//             }
//         }
//     }
// }

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
        for (c=0; c < FIL_2; c++) {
            for (h=0; h < H_2; h++) {
                for (w=0; w < W_2; w++) {
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
        for (c=0; c < FIL_2; c++) {
            for (h=0; h < H_2; h++) {
                for (w=0; w < W_2; w++) {
                    out[n][c][h][w] = in[n][c][h][w] + fx[n][c][h][w];
                }
            }
        }
    }
}