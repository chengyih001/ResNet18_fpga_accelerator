#include "model.h"

//void resnet18_forward(data_t batch_X[N][C][H_0][W_0],
//                      int batch_y[N],
//                      float* accuracy)
void resnet18_forward(data_t batch_X[N][C][H_0][W_0],
                      int batch_y[N],
                      data_t (*out)[N][CLASS])
{   
#pragma HLS INTERFACE m_axi depth=512 port = batch_X max_read_burst_length=256 max_write_burst_length=256 max_widen_bitwidth=512
#pragma HLS INTERFACE m_axi depth=512 port = batch_y max_read_burst_length=256 max_write_burst_length=256 max_widen_bitwidth=512

//     // #pragma HLS STREAM variable=conv_out_0

//     // #pragma HLS STREAM variable=conv_out_1_1
//     // #pragma HLS STREAM variable=conv_out_1_2
//     // #pragma HLS STREAM variable=residual_out_1_2

//     // #pragma HLS STREAM variable=conv_out_2_1
//     // #pragma HLS STREAM variable=conv_out_2_2
//     // #pragma HLS STREAM variable=residual_out_2_2

//     // #pragma HLS STREAM variable=conv_out_3_1
//     // #pragma HLS STREAM variable=conv_out_3_2
//     // #pragma HLS STREAM variable=residual_out_3_2

//     // #pragma HLS STREAM variable=conv_out_4_1
//     // #pragma HLS STREAM variable=conv_out_4_2
//     // #pragma HLS STREAM variable=residual_out_4_2

//     // #pragma HLS STREAM variable=conv_out_5_1
//     // #pragma HLS STREAM variable=conv_out_5_2
//     // #pragma HLS STREAM variable=residual_out_5_2

//     // #pragma HLS STREAM variable=conv_out_6_1
//     // #pragma HLS STREAM variable=conv_out_6_2
//     // #pragma HLS STREAM variable=residual_out_6_2

//     // #pragma HLS STREAM variable=conv_out_7_1
//     // #pragma HLS STREAM variable=conv_out_7_2
//     // #pragma HLS STREAM variable=residual_out_7_2

//     // #pragma HLS STREAM variable=conv_out_8_1
//     // #pragma HLS STREAM variable=conv_out_8_2
//     // #pragma HLS STREAM variable=residual_out_8_2

//     // #pragma HLS STREAM variable=conv_out_9_1
//     // #pragma HLS STREAM variable=conv_out_9_2
//     // #pragma HLS STREAM variable=residual_out_9_2


    // initial - in: [N][3][32][32]; out: [N][16][32][32]
    data_t conv_out_0[N][FIL_0][H_0][W_0];
    data_t bn_out_0[N][FIL_0][H_0][W_0];
    data_t relu_out_0[N][FIL_0][H_0][W_0];
    conv_forward_0(conv_out_0, batch_X, conv_weight_0, conv_bias_0);
    bn_forward_0(bn_out_0, conv_out_0, bn_mean_0, bn_var_0, bn_gamma_0, bn_beta_0, 0.00001);
    relu_forward_0(relu_out_0, bn_out_0);

    // block 1 - in: [N][16][32][32]; out: [N][16][32][32]
    data_t conv_out_1_1[N][FIL_0][H_0][W_0];
    data_t bn_out_1_1[N][FIL_0][H_0][W_0];
    data_t relu_out_1_1[N][FIL_0][H_0][W_0];
    data_t conv_out_1_2[N][FIL_0][H_0][W_0];
    data_t bn_out_1_2[N][FIL_0][H_0][W_0];
    data_t residual_out_1_2[N][FIL_0][H_0][W_0];
    data_t relu_out_1_2[N][FIL_0][H_0][W_0];
    conv_forward_block_1(conv_out_1_1, relu_out_0, conv_weight_1_1, conv_bias_1_1);
    bn_forward_0(bn_out_1_1, conv_out_1_1, bn_mean_1_1, bn_var_1_1, bn_gamma_1_1, bn_beta_1_1, 0.00001);
    relu_forward_0(relu_out_1_1, bn_out_1_1);
    conv_forward_block_1(conv_out_1_2, relu_out_1_1, conv_weight_1_2, conv_bias_1_2);
    bn_forward_0(bn_out_1_2, conv_out_1_2, bn_mean_1_2, bn_var_1_2, bn_gamma_1_2, bn_beta_1_2, 0.00001);
    residual_forward_0(residual_out_1_2, relu_out_0, bn_out_1_2);
    relu_forward_0(relu_out_1_2, residual_out_1_2);

    // block 2 - in: [N][16][32][32]; out: [N][16][32][32]
    data_t conv_out_2_1[N][FIL_0][H_0][W_0];
    data_t bn_out_2_1[N][FIL_0][H_0][W_0];
    data_t relu_out_2_1[N][FIL_0][H_0][W_0];
    data_t conv_out_2_2[N][FIL_0][H_0][W_0];
    data_t bn_out_2_2[N][FIL_0][H_0][W_0];
    data_t residual_out_2_2[N][FIL_0][H_0][W_0];
    data_t relu_out_2_2[N][FIL_0][H_0][W_0];
    conv_forward_block_1(conv_out_2_1, relu_out_1_2, conv_weight_2_1, conv_bias_2_1);
    bn_forward_0(bn_out_2_1, conv_out_2_1, bn_mean_2_1, bn_var_2_1, bn_gamma_2_1, bn_beta_2_1, 0.00001);
    relu_forward_0(relu_out_2_1, bn_out_2_1);
    conv_forward_block_1(conv_out_2_2, relu_out_2_1, conv_weight_2_2, conv_bias_2_2);
    bn_forward_0(bn_out_2_2, conv_out_2_2, bn_mean_2_2, bn_var_2_2, bn_gamma_2_2, bn_beta_2_2, 0.00001);
    residual_forward_0(residual_out_2_2, relu_out_1_2, bn_out_2_2);
    relu_forward_0(relu_out_2_2, residual_out_2_2);

    // block 3 - in: [N][16][32][32]; out: [N][32][16][16]
    data_t conv_out_3_1[N][FIL_1][H_1][W_1];
    data_t bn_out_3_1[N][FIL_1][H_1][W_1];
    data_t relu_out_3_1[N][FIL_1][H_1][W_1];
    data_t conv_out_3_2[N][FIL_1][H_1][W_1];
    data_t bn_out_3_2[N][FIL_1][H_1][W_1];
    data_t residual_out_3_2[N][FIL_1][H_1][W_1];
    data_t relu_out_3_2[N][FIL_1][H_1][W_1];
    conv_forward_block_1_1(conv_out_3_1, relu_out_2_2, conv_weight_3_1, conv_bias_3_1);
    bn_forward_1(bn_out_3_1, conv_out_3_1, bn_mean_3_1, bn_var_3_1, bn_gamma_3_1, bn_beta_3_1, 0.00001);
    relu_forward_1(relu_out_3_1, bn_out_3_1);
    conv_forward_block_2(conv_out_3_2, relu_out_3_1, conv_weight_3_2, conv_bias_3_2);
    bn_forward_1(bn_out_3_2, conv_out_3_2, bn_mean_3_2, bn_var_3_2, bn_gamma_3_2, bn_beta_3_2, 0.00001);
    residual_forward_0_1(residual_out_3_2, relu_out_2_2, bn_out_3_2,
                        conv_weight_3_1, conv_bias_3_1, bn_mean_3_1,
                        bn_var_3_1, bn_gamma_3_1, bn_beta_3_1);
    relu_forward_1(relu_out_3_2, residual_out_3_2);

    // block 4 - in: [N][32][16][16]; out: [N][32][16][16]
    data_t conv_out_4_1[N][FIL_1][H_1][W_1];
    data_t bn_out_4_1[N][FIL_1][H_1][W_1];
    data_t relu_out_4_1[N][FIL_1][H_1][W_1];
    data_t conv_out_4_2[N][FIL_1][H_1][W_1];
    data_t bn_out_4_2[N][FIL_1][H_1][W_1];
    data_t residual_out_4_2[N][FIL_1][H_1][W_1];
    data_t relu_out_4_2[N][FIL_1][H_1][W_1];
    conv_forward_block_2(conv_out_4_1, relu_out_3_2, conv_weight_4_1, conv_bias_4_1);
    bn_forward_1(bn_out_4_1, conv_out_4_1, bn_mean_4_1, bn_var_4_1, bn_gamma_4_1, bn_beta_4_1, 0.00001);
    relu_forward_1(relu_out_4_1, bn_out_4_1);
    conv_forward_block_2(conv_out_4_2, relu_out_4_1, conv_weight_4_2, conv_bias_4_2);
    bn_forward_1(bn_out_4_2, conv_out_4_2, bn_mean_4_2, bn_var_4_2, bn_gamma_4_2, bn_beta_4_2, 0.00001);
    residual_forward_1(residual_out_4_2, relu_out_3_2, bn_out_4_2);
    relu_forward_1(relu_out_4_2, residual_out_4_2);

    // block 5 - in: [N][32][16][16]; out: [N][32][16][16]
    data_t conv_out_5_1[N][FIL_1][H_1][W_1];
    data_t bn_out_5_1[N][FIL_1][H_1][W_1];
    data_t relu_out_5_1[N][FIL_1][H_1][W_1];
    data_t conv_out_5_2[N][FIL_1][H_1][W_1];
    data_t bn_out_5_2[N][FIL_1][H_1][W_1];
    data_t residual_out_5_2[N][FIL_1][H_1][W_1];
    data_t relu_out_5_2[N][FIL_1][H_1][W_1];
    conv_forward_block_2(conv_out_5_1, relu_out_4_2, conv_weight_5_1, conv_bias_5_1);
    bn_forward_1(bn_out_5_1, conv_out_5_1, bn_mean_5_1, bn_var_5_1, bn_gamma_5_1, bn_beta_5_1, 0.00001);
    relu_forward_1(relu_out_5_1, bn_out_5_1);
    conv_forward_block_2(conv_out_5_2, relu_out_5_1, conv_weight_5_2, conv_bias_5_2);
    bn_forward_1(bn_out_5_2, conv_out_5_2, bn_mean_5_2, bn_var_5_2, bn_gamma_5_2, bn_beta_5_2, 0.00001);
    residual_forward_1(residual_out_5_2, relu_out_4_2, bn_out_5_2);
    relu_forward_1(relu_out_5_2, residual_out_5_2);

    // block 6 - in: [N][32][16][16]; out: [N][64][8][8]
    data_t conv_out_6_1[N][FIL_2][H_2][W_2];
    data_t bn_out_6_1[N][FIL_2][H_2][W_2];
    data_t relu_out_6_1[N][FIL_2][H_2][W_2];
    data_t conv_out_6_2[N][FIL_2][H_2][W_2];
    data_t bn_out_6_2[N][FIL_2][H_2][W_2];
    data_t residual_out_6_2[N][FIL_2][H_2][W_2];
    data_t relu_out_6_2[N][FIL_2][H_2][W_2];
    conv_forward_block_2_1(conv_out_6_1, relu_out_5_2, conv_weight_6_1, conv_bias_6_1);
    bn_forward_2(bn_out_6_1, conv_out_6_1, bn_mean_6_1, bn_var_6_1, bn_gamma_6_1, bn_beta_6_1, 0.00001);
    relu_forward_2(relu_out_6_1, bn_out_6_1);
    conv_forward_block_3(conv_out_6_2, relu_out_6_1, conv_weight_6_2, conv_bias_6_2);
    bn_forward_2(bn_out_6_2, conv_out_6_2, bn_mean_6_2, bn_var_6_2, bn_gamma_6_2, bn_beta_6_2, 0.00001);
    residual_forward_1_1(residual_out_6_2, relu_out_5_2, bn_out_6_2,
                        conv_weight_6_1, conv_bias_6_1, bn_mean_6_1,
                        bn_var_6_1, bn_gamma_6_1, bn_beta_6_1);
    relu_forward_2(relu_out_6_2, residual_out_6_2);

    // block 7 - in: [N][64][8][8]; out: [N][64][8][8]
    data_t conv_out_7_1[N][FIL_2][H_2][W_2];
    data_t bn_out_7_1[N][FIL_2][H_2][W_2];
    data_t relu_out_7_1[N][FIL_2][H_2][W_2];
    data_t conv_out_7_2[N][FIL_2][H_2][W_2];
    data_t bn_out_7_2[N][FIL_2][H_2][W_2];
    data_t residual_out_7_2[N][FIL_2][H_2][W_2];
    data_t relu_out_7_2[N][FIL_2][H_2][W_2];
    conv_forward_block_3(conv_out_7_1, relu_out_6_2, conv_weight_7_1, conv_bias_7_1);
    bn_forward_2(bn_out_7_1, conv_out_7_1, bn_mean_7_1, bn_var_7_1, bn_gamma_7_1, bn_beta_7_1, 0.00001);
    relu_forward_2(relu_out_7_1, bn_out_7_1);
    conv_forward_block_3(conv_out_7_2, relu_out_7_1, conv_weight_7_2, conv_bias_7_2);
    bn_forward_2(bn_out_7_2, conv_out_7_2, bn_mean_7_2, bn_var_7_2, bn_gamma_7_2, bn_beta_7_2, 0.00001);
    residual_forward_2(residual_out_7_2, relu_out_6_2, bn_out_7_2);
    relu_forward_2(relu_out_7_2, residual_out_7_2);

    // block 8 - in: [N][64][8][8]; out: [N][64][8][8]
    data_t conv_out_8_1[N][FIL_2][H_2][W_2];
    data_t bn_out_8_1[N][FIL_2][H_2][W_2];
    data_t relu_out_8_1[N][FIL_2][H_2][W_2];
    data_t conv_out_8_2[N][FIL_2][H_2][W_2];
    data_t bn_out_8_2[N][FIL_2][H_2][W_2];
    data_t residual_out_8_2[N][FIL_2][H_2][W_2];
    data_t relu_out_8_2[N][FIL_2][H_2][W_2];
    conv_forward_block_3(conv_out_8_1, relu_out_7_2, conv_weight_8_1, conv_bias_8_1);
    bn_forward_2(bn_out_8_1, conv_out_8_1, bn_mean_8_1, bn_var_8_1, bn_gamma_8_1, bn_beta_8_1, 0.00001);
    relu_forward_2(relu_out_8_1, bn_out_8_1);
    conv_forward_block_3(conv_out_8_2, relu_out_8_1, conv_weight_8_2, conv_bias_8_2);
    bn_forward_2(bn_out_8_2, conv_out_8_2, bn_mean_8_2, bn_var_8_2, bn_gamma_8_2, bn_beta_8_2, 0.00001);
    residual_forward_2(residual_out_8_2, relu_out_7_2, bn_out_8_2);
    relu_forward_2(relu_out_8_2, residual_out_8_2);

    // block 9 - in: [N][64][8][8]; out: [N][64][8][8]
    data_t conv_out_9_1[N][FIL_2][H_2][W_2];
    data_t bn_out_9_1[N][FIL_2][H_2][W_2];
    data_t relu_out_9_1[N][FIL_2][H_2][W_2];
    data_t conv_out_9_2[N][FIL_2][H_2][W_2];
    data_t bn_out_9_2[N][FIL_2][H_2][W_2];
    data_t residual_out_9_2[N][FIL_2][H_2][W_2];
    data_t relu_out_9_2[N][FIL_2][H_2][W_2];
    conv_forward_block_3(conv_out_9_1, relu_out_8_2, conv_weight_9_1, conv_bias_9_1);
    bn_forward_2(bn_out_9_1, conv_out_9_1, bn_mean_9_1, bn_var_9_1, bn_gamma_9_1, bn_beta_9_1, 0.00001);
    relu_forward_2(relu_out_9_1, bn_out_9_1);
    conv_forward_block_3(conv_out_9_2, conv_out_9_2, conv_weight_9_2, conv_bias_9_2);
    bn_forward_2(bn_out_9_2, conv_out_9_2, bn_mean_9_2, bn_var_9_2, bn_gamma_9_2, bn_beta_9_2, 0.00001);
    residual_forward_2(residual_out_9_2, relu_out_8_2, bn_out_9_2);
    relu_forward_2(relu_out_9_2, residual_out_9_2);

    // final - in: [N][64][8][8]; out: [N][class_size]
    data_t gap_out[N][FIL][OUT_H][OUT_W];
    data_t ma_reshape_out[N][D];
    data_t ma_multi_out[N][CLASS];
    data_t score[N][CLASS];
    gap_forward(gap_out, relu_out_9_2);
    matrix_reshape(ma_reshape_out, gap_out);
    matrix_multiplication(ma_multi_out, ma_reshape_out, fc_weight);
    full_connected_forward(score, ma_multi_out, fc_bias);

	for (int n=0; n < N; n++) {
		for (int c=0; c < CLASS; c++) {
			printf("%f, ", score);
			(*out)[n][c] = score[n][c];
		}
	}



//    // check accuracy
//    int y_pred[N];
//    data_t max_score = 0.0;
//    for (int i=0; i < N; i++) {
//#pragma HLS LOOP_FLATTEN
//        max_score = score[i][0];
//        y_pred[i] = 0;
//        for (int j=1; j < CLASS; j++) {
//#pragma HLS UNROLL
//            if (score[i][j] > max_score) {
//                max_score = score[i][j];
//                y_pred[i] = j;
//            }
//        }
//    }
//
//    float sum = 0.0;
//    float total = 0.0;
//    for (int i=0; i < N; i++) {
//#pragma HLS UNROLL
//        total += 1.0;
//        if (y_pred[i] == batch_y[i]) {
//            sum += 1.0;
//        }
//    }
//    *accuracy = sum/total;
}
