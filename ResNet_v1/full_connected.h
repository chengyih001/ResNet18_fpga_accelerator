#pragma once
#include "params.h"

// x: [N][64][1][1]; x_reshape: [N][64]
void matrix_reshape(data_t out[N][D], data_t in_x[N][FC_FIL_0][FC_H_0][FC_W_0]);

// w: [10][64]; w_transform: [64][10]
void matrix_transformation(data_t out[D][CLASS], data_t in_w[CLASS][D]);

// x_reshape: [N][64]; w_transform: [64][10]; output: [N][10]
void matrix_multiplication(data_t out[N][CLASS], data_t in_x[N][D], const data_t in_w[D][CLASS]);

// in: [N][10]; bias: [10]; output: [N][10]
void full_connected_forward(data_t out[N][CLASS], data_t in[N][CLASS], const data_t bias[CLASS]);