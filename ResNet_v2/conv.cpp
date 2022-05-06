#include "conv.h"

/*
 * x: [N][3][32][32]
 * w: [16][3][3][3]
 * b: [16]
 * 
 * output: [N][16][32][32]
 * 
 * stride 1, pad 1
 */
void conv_forward_0(data_t out[N][FIL_0][H_0][W_0],
                    data_t in[N][C][H_0][W_0], 
                    const data_t weight[FIL_0][C][WEI_H][WEI_W],
                    const data_t bias[FIL_0])
{
    int n, c, h, w, f, hh, ww;
    // pad x
    data_t in_pad[N][C][H_0_PAD][W_0_PAD];
    conv_forward_0_label0:for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < C; c++) {
            for (h=0; h < H_0; h++) {
                for (w=0; w < W_0; w++) {
#pragma HLS UNROLL
                    in_pad[n][c][h+PAD_0][w+PAD_0] = in[n][c][h][w];
                }
            }
        }
    }

    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (f=0; f < FIL_0; f++) {
            for (h=0; h < H_0; h++) {
                for (w=0; w < W_0; w++) {
#pragma HLS UNROLL
                    out[n][f][h][w] = bias[f];
                }
            }
        }
    }

    data_t temp;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (f=0; f < FIL_0; f++) {
            for (h=0; h < H_0; h++) {
                for (w=0; w < W_0; w++) {
                    for (c=0; c < C; c++) {
                        for (hh=0; hh < WEI_H; hh++) {
                            for (ww=0; ww < WEI_W; ww++) {
#pragma HLS UNROLL
                            	temp = out[n][f][h][w];
                                out[n][f][h][w] = temp + weight[f][c][hh][ww] * in_pad[n][c][h*STR_0+hh][w*STR_0+ww];
                            }
                        }
                    }
                }
            }
        }
    }

}

/*
 * x: [N][16][32][32]
 * w: [16][16][3][3]
 * b: [16]
 *
 * output: [N][16][32][32]
 *
 * stride 1, pad 1
 */
void conv_forward_block_1(data_t out[N][FIL_0][H_0][W_0],
                    data_t in[N][FIL_0][H_0][W_0],
                    const data_t weight[FIL_0][FIL_0][WEI_H][WEI_W],
                    const data_t bias[FIL_0])
{
    int n, c, h, w, f, hh, ww;
    // pad x
    data_t in_pad[N][FIL_0][H_0_PAD][W_0_PAD];
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL_0; c++) {
            for (h=0; h < H_0; h++) {
                for (w=0; w < W_0; w++) {
#pragma HLS UNROLL
                    in_pad[n][c][h+PAD_0][w+PAD_0] = in[n][c][h][w];
                }
            }
        }
    }

    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (f=0; f < FIL_0; f++) {
            for (h=0; h < H_0; h++) {
                for (w=0; w < W_0; w++) {
#pragma HLS UNROLL
                    out[n][f][h][w] = bias[f];
                }
            }
        }
    }

    data_t temp;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (f=0; f < FIL_0; f++) {
            for (h=0; h < H_0; h++) {
                for (w=0; w < W_0; w++) {
                    for (c=0; c < FIL_0; c++) {
                        for (hh=0; hh < WEI_H; hh++) {
                            for (ww=0; ww < WEI_W; ww++) {
#pragma HLS UNROLL
                            	temp = out[n][f][h][w];
                                out[n][f][h][w] = temp + weight[f][c][hh][ww] * in_pad[n][c][h*STR_0+hh][w*STR_0+ww];
                            }
                        }
                    }
                }
            }
        }
    }

}

/*
 * x: [N][16][32][32]
 * w: [32][16][3][3]
 * b: [32]
 * 
 * output: [N][32][16][16]
 * 
 * stride 2, pad 1
 */
void conv_forward_block_1_1(data_t out[N][FIL_1][H_1][W_1],
                    data_t in[N][FIL_0][H_0][W_0], 
                    const data_t weight[FIL_1][FIL_0][WEI_H][WEI_W], 
                    const data_t bias[FIL_1])
{
    int n, c, h, w, f, hh, ww;
    // pad x
    data_t in_pad[N][FIL_0][H_0_PAD][W_0_PAD];
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL_0; c++) {
            for (h=0; h < H_0; h++) {
                for (w=0; w < W_0; w++) {
#pragma HLS UNROLL
                    in_pad[n][c][h+PAD_0][w+PAD_0] = in[n][c][h][w];
                }
            }
        }
    }

    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (f=0; f < FIL_1; f++) {
            for (h=0; h < H_1; h++) {
                for (w=0; w < W_1; w++) {
#pragma HLS UNROLL
                    out[n][f][h][w] = bias[f];
                }
            }
        }
    }

    data_t temp;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (f=0; f < FIL_1; f++) {
            for (h=0; h < H_1; h++) {
                for (w=0; w < W_1; w++) {
                	for (c=0; c < FIL_0; c++) {
						for (hh=0; hh < WEI_H; hh++) {
							for (ww=0; ww < WEI_W; ww++) {
#pragma HLS UNROLL
								temp = out[n][f][h][w];
								out[n][f][h][w] = temp + weight[f][c][hh][ww] * in_pad[n][c][h*STR_1+hh][w*STR_1+ww];
							}
						}
					}
                }
            }
        }
    }
}

/*
 * x: [N][32][16][16]
 * w: [32][32][3][3]
 * b: [32]
 * 
 * output: [N][32][16][16]
 * 
 * stride 1, pad 1
 */
void conv_forward_block_2(data_t out[N][FIL_1][H_1][W_1],
                    data_t in[N][FIL_1][H_1][W_1], 
                    const data_t weight[FIL_1][FIL_1][WEI_H][WEI_W], 
                    const data_t bias[FIL_1])
{
    int n, c, h, w, f, hh, ww;
    // pad x
    data_t in_pad[N][FIL_1][H_1_PAD][W_1_PAD];
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL_1; c++) {
            for (h=0; h < H_1; h++) {
                for (w=0; w < W_1; w++) {
#pragma HLS UNROLL
                    in_pad[n][c][h+PAD_0][w+PAD_0] = in[n][c][h][w];
                }
            }
        }
    }

    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (f=0; f < FIL_1; f++) {
            for (h=0; h < H_1; h++) {
                for (w=0; w < W_1; w++) {
#pragma HLS UNROLL
                    out[n][f][h][w] = bias[f];
                }
            }
        }
    }

    data_t temp;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (f=0; f < FIL_1; f++) {
            for (h=0; h < H_1; h++) {
                for (w=0; w < W_1; w++) {
                    for (c=0; c < FIL_1; c++) {
                        for (hh=0; hh < WEI_H; hh++) {
                            for (ww=0; ww < WEI_W; ww++) {
#pragma HLS UNROLL
                            	temp = out[n][f][h][w];
                                out[n][f][h][w] = temp + weight[f][c][hh][ww] * in_pad[n][c][h*STR_0+hh][w*STR_0+ww];
                            }
                        }
                    }
                }
            }
        }
    }
}

/*
 * x: [N][32][16][16]
 * w: [64][32][3][3]
 * b: [64]
 * 
 * output: [N][64][8][8]
 * 
 * stride 2, pad 1
 */
void conv_forward_block_2_1(data_t out[N][FIL_2][H_2][W_2],
                    data_t in[N][FIL_1][H_1][W_1], 
                    const data_t weight[FIL_2][FIL_1][WEI_H][WEI_W], 
                    const data_t bias[FIL_2])
{
    int n, c, h, w, f, hh, ww;
    // pad x
    data_t in_pad[N][FIL_1][H_1_PAD][W_1_PAD];
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL_1; c++) {
            for (h=0; h < H_1; h++) {
                for (w=0; w < W_1; w++) {
#pragma HLS UNROLL
                    in_pad[n][c][h+PAD_0][w+PAD_0] = in[n][c][h][w];
                }
            }
        }
    }

    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (f=0; f < FIL_2; f++) {
            for (h=0; h < H_2; h++) {
                for (w=0; w < W_2; w++) {
#pragma HLS UNROLL
                    out[n][f][h][w] = bias[f];
                }
            }
        }
    }

    data_t temp;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (f=0; f < FIL_2; f++) {
            for (h=0; h < H_2; h++) {
                for (w=0; w < W_2; w++) {
                    for (c=0; c < FIL_1; c++) {
                        for (hh=0; hh < WEI_H; hh++) {
                            for (ww=0; ww < WEI_W; ww++) {
#pragma HLS UNROLL
                            	temp = out[n][f][h][w];
                                out[n][f][h][w] = temp + weight[f][c][hh][ww] * in_pad[n][c][h*STR_1+hh][w*STR_1+ww];
                            }
                        }
                    }
                }
            }
        }
    }

}

/*
 * x: [N][64][8][8]
 * w: [64][64][3][3]
 * b: [64]
 * 
 * output: [N][64][8][8]
 * 
 * stride 1, pad 1
 */
void conv_forward_block_3(data_t out[N][FIL_2][H_2][W_2],
                    data_t in[N][FIL_2][H_2][W_2], 
                    const data_t weight[FIL_2][FIL_2][WEI_H][WEI_W], 
                    const data_t bias[FIL_2])
{
    int n, c, h, w, f, hh, ww;
    // pad x
    data_t in_pad[N][FIL_2][H_2_PAD][W_2_PAD];
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL_2; c++) {
            for (h=0; h < H_2; h++) {
                for (w=0; w < W_2; w++) {
#pragma HLS UNROLL
                    in_pad[n][c][h+PAD_0][w+PAD_0] = in[n][c][h][w];
                }
            }
        }
    }

    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (f=0; f < FIL_2; f++) {
            for (h=0; h < H_2; h++) {
                for (w=0; w < W_2; w++) {
#pragma HLS UNROLL
                    out[n][f][h][w] = bias[f];
                }
            }
        }
    }
    data_t temp;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (f=0; f < FIL_2; f++) {
            for (h=0; h < H_2; h++) {
                for (w=0; w < W_2; w++) {
                    for (c=0; c < FIL_2; c++) {
                        for (hh=0; hh < WEI_H; hh++) {
                            for (ww=0; ww < WEI_W; ww++) {
#pragma HLS UNROLL
                            	temp = out[n][f][h][w];
                                out[n][f][h][w] = temp + weight[f][c][hh][ww] * in_pad[n][c][h*STR_0+hh][w*STR_0+ww];
                            }
                        }
                    }
                }
            }
        }
    }

}

