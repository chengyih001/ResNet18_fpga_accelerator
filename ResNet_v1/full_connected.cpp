#include "full_connected.h"

/*
 * x: [N][64][1][1]
 * 
 * x_reshape: [N][64]
 */
void matrix_reshape(data_t out[N][D],
                    data_t in_x[N][FC_FIL_0][FC_H_0][FC_W_0])
{
    int n, c, h, w;
    for (n=0; n < N; n++) {
        for (c=0; c < FC_FIL_0; c++) {
            for (h=0; h < FC_H_0; h++) {
                for (w=0; w < FC_W_0; w++) {
                    out[n][c * FC_H_0 * FC_W_0 + h * FC_W_0 + w] = in_x[n][c][h][w];
                }
            }
        }
    }
}

/*
 * w: [10][64]
 * 
 * w_transform: [64][10]
 */
void matrix_transformation(data_t out[D][CLASS],
                           data_t in_w[CLASS][D])
{
    int m, d;
    for (d=0; d < FC_FIL_0*FC_H_0*FC_W_0; d++) {
        for (m=0; m < CLASS; m++) {
            out[d][m] = in_w[m][d];
        }
    }
}

/*
 * x_reshape: [N][64]
 * w_transform: [64][10]
 * 
 * output: [N][10]
 */
void matrix_multiplication(data_t out[N][CLASS],
                           data_t in_x[N][D],
                           const data_t in_w[D][CLASS])
{
    int n, d, m;
    for (n=0; n < N; n++) {
        for (m=0; m < CLASS; m++) {
            for (d=0; d < D; d++) {
                out[n][m] = out[n][m] + in_x[n][d] * in_w[d][m];
            }
        }
    }
}

/*
 * in: [N][10]
 * bias: [10]
 * 
 * output: [N][10]
 */
void full_connected_forward(data_t out[N][CLASS],
                            data_t in[N][CLASS],
                            const data_t bias[CLASS])
{
    int n, m;
    for (n=0; n < N; n++) {
        for (m=0; m < CLASS; m++) {
            out[n][m] = in[n][m] + bias[m];
        }
    }
}