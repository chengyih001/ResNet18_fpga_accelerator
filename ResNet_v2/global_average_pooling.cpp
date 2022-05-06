#include "global_average_pooling.h"

/*
 * x: [N][64][8][8]
 * 
 * output: [N][64][1][1]
 */
void gap_forward(data_t out[N][FIL][OUT_H][OUT_W],
                 data_t in[N][FIL][IN_H][IN_W])
{
    int n, c, h, w;
    data_t in_buffer[N][FIL][OUT_H][OUT_W];
#pragma HLS ARRAY_PARTITION variable=in_buffer complete dim=3
    memcpy(in_buffer, in, N*FIL*OUT_H*OUT_W*sizeof(data_t));
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL; c++) {
            for (h=0; h < IN_H; h++) {
                for (w=0; w < IN_W; w++) {
#pragma HLS UNROLL
                    out[n][c][0][0] = out[n][c][0][0] + in[n][c][h][w];
                }
            }
        }
    }

    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < FIL; c++) {
#pragma HLS UNROLL
        	out[n][c][0][0] = out[n][c][0][0] / (IN_H * IN_W);
        }
    }
}


