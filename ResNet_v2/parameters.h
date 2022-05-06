#pragma once
#include "params.h"

// initial layer - in: [N][3][32][32]; out: [N][16][32][32]
const data_t conv_weight_0[FIL_0][C][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_0.h"
};
const data_t conv_bias_0[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_0.h"
};

const data_t bn_mean_0[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_0.h"
};
const data_t bn_var_0[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_0.h"
};
const data_t bn_gamma_0[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_0.h"
};
const data_t bn_beta_0[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_0.h"
};


// block 1 - in: [N][16][32][32]; out: [N][16][32][32]
const data_t conv_weight_1_1[FIL_0][FIL_0][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_1_1.h"
};
const data_t conv_bias_1_1[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_1_1.h"
};

const data_t bn_mean_1_1[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_1_1.h"
};
const data_t bn_var_1_1[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_1_1.h"
};
const data_t bn_gamma_1_1[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_1_1.h"
};
const data_t bn_beta_1_1[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_1_1.h"
};

const data_t conv_weight_1_2[FIL_0][FIL_0][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_1_2.h"
};
const data_t conv_bias_1_2[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_1_2.h"
};

const data_t bn_mean_1_2[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_1_2.h"
};
const data_t bn_var_1_2[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_1_2.h"
};
const data_t bn_gamma_1_2[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_1_2.h"
};
const data_t bn_beta_1_2[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_1_2.h"
};

// block 2 - in: [N][16][32][32]; out: [N][16][32][32]
const data_t conv_weight_2_1[FIL_0][FIL_0][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_2_1.h"
};
const data_t conv_bias_2_1[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_2_1.h"
};

const data_t bn_mean_2_1[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_2_1.h"
};
const data_t bn_var_2_1[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_2_1.h"
};
const data_t bn_gamma_2_1[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_2_1.h"
};
const data_t bn_beta_2_1[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_2_1.h"
};

const data_t conv_weight_2_2[FIL_0][FIL_0][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_2_2.h"
};
const data_t conv_bias_2_2[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_2_2.h"
};

const data_t bn_mean_2_2[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_2_2.h"
};
const data_t bn_var_2_2[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_2_2.h"
};
const data_t bn_gamma_2_2[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_2_2.h"
};
const data_t bn_beta_2_2[FIL_0] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_2_2.h"
};


// block 3 - in: [N][16][32][32]; out: [N][32][16][16]
const data_t conv_weight_3_1[FIL_1][FIL_0][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_3_1.h"
};
const data_t conv_bias_3_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_3_1.h"
};

const data_t bn_mean_3_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_3_1.h"
};
const data_t bn_var_3_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_3_1.h"
};
const data_t bn_gamma_3_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_3_1.h"
};
const data_t bn_beta_3_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_3_1.h"
};

const data_t conv_weight_3_2[FIL_1][FIL_1][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_3_2.h"
};
const data_t conv_bias_3_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_3_2.h"
};

const data_t bn_mean_3_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_3_2.h"
};
const data_t bn_var_3_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_3_2.h"
};
const data_t bn_gamma_3_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_3_2.h"
};
const data_t bn_beta_3_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_3_2.h"
};

// block 4 - in: [N][32][16][16]; out: [N][32][16][16]
const data_t conv_weight_4_1[FIL_1][FIL_1][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_4_1.h"
};
const data_t conv_bias_4_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_4_1.h"
};

const data_t bn_mean_4_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_4_1.h"
};
const data_t bn_var_4_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_4_1.h"
};
const data_t bn_gamma_4_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_4_1.h"
};
const data_t bn_beta_4_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_4_1.h"
};

const data_t conv_weight_4_2[FIL_1][FIL_1][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_4_2.h"
};
const data_t conv_bias_4_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_4_2.h"
};

const data_t bn_mean_4_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_4_2.h"
};
const data_t bn_var_4_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_4_2.h"
};
const data_t bn_gamma_4_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_4_2.h"
};
const data_t bn_beta_4_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_4_2.h"
};

// block 5 - in: [N][32][16][16]; out: [N][32][16][16]
const data_t conv_weight_5_1[FIL_1][FIL_1][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_5_1.h"
};
const data_t conv_bias_5_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_5_1.h"
};

const data_t bn_mean_5_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_5_1.h"
};
const data_t bn_var_5_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_5_1.h"
};
const data_t bn_gamma_5_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_5_1.h"
};
const data_t bn_beta_5_1[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_5_1.h"
};

const data_t conv_weight_5_2[FIL_1][FIL_1][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_5_2.h"
};
const data_t conv_bias_5_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_5_2.h"
};

const data_t bn_mean_5_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_5_2.h"
};
const data_t bn_var_5_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_5_2.h"
};
const data_t bn_gamma_5_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_5_2.h"
};
const data_t bn_beta_5_2[FIL_1] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_5_2.h"
};

// block 6 - in: [N][32][16][16]; out: [N][64][8][8]
const data_t conv_weight_6_1[FIL_2][FIL_1][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_6_1.h"
};
const data_t conv_bias_6_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_6_1.h"
};

const data_t bn_mean_6_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_6_1.h"
};
const data_t bn_var_6_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_6_1.h"
};
const data_t bn_gamma_6_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_6_1.h"
};
const data_t bn_beta_6_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_6_1.h"
};

const data_t conv_weight_6_2[FIL_2][FIL_2][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_6_2.h"
};
const data_t conv_bias_6_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_6_2.h"
};

const data_t bn_mean_6_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_6_2.h"
};
const data_t bn_var_6_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_6_2.h"
};
const data_t bn_gamma_6_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_6_2.h"
};
const data_t bn_beta_6_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_6_2.h"
};


// block 7 - in: [N][64][8][8]; out: [N][64][8][8]
const data_t conv_weight_7_1[FIL_2][FIL_2][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_7_1.h"
};
const data_t conv_bias_7_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_7_1.h"
};

const data_t bn_mean_7_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_7_1.h"
};
const data_t bn_var_7_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_7_1.h"
};
const data_t bn_gamma_7_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_7_1.h"
};
const data_t bn_beta_7_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_7_1.h"
};

const data_t conv_weight_7_2[FIL_2][FIL_2][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_7_2.h"
};
const data_t conv_bias_7_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_7_2.h"
};

const data_t bn_mean_7_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_7_2.h"
};
const data_t bn_var_7_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_7_2.h"
};
const data_t bn_gamma_7_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_7_2.h"
};
const data_t bn_beta_7_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_7_2.h"
};

// block 8 - in: [N][64][8][8]; out: [N][64][8][8]
const data_t conv_weight_8_1[FIL_2][FIL_2][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_8_1.h"
};
const data_t conv_bias_8_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_8_1.h"
};

const data_t bn_mean_8_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_8_1.h"
};
const data_t bn_var_8_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_8_1.h"
};
const data_t bn_gamma_8_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_8_1.h"
};
const data_t bn_beta_8_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_8_1.h"
};

const data_t conv_weight_8_2[FIL_2][FIL_2][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_8_2.h"
};
const data_t conv_bias_8_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_8_2.h"
};

const data_t bn_mean_8_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_8_2.h"
};
const data_t bn_var_8_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_8_2.h"
};
const data_t bn_gamma_8_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_8_2.h"
};
const data_t bn_beta_8_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_8_2.h"
};

// block 9 - in: [N][64][8][8]; out: [N][64][8][8]
const data_t conv_weight_9_1[FIL_2][FIL_2][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_9_1.h"
};
const data_t conv_bias_9_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_9_1.h"
};

const data_t bn_mean_9_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_9_1.h"
};
const data_t bn_var_9_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_9_1.h"
};
const data_t bn_gamma_9_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_9_1.h"
};
const data_t bn_beta_9_1[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_9_1.h"
};

const data_t conv_weight_9_2[FIL_2][FIL_2][WEI_H][WEI_W] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_weight_9_2.h"
};
const data_t conv_bias_9_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\conv_bias_9_2.h"
};

const data_t bn_mean_9_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_mean_9_2.h"
};
const data_t bn_var_9_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_var_9_2.h"
};
const data_t bn_gamma_9_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_gamma_9_2.h"
};
const data_t bn_beta_9_2[FIL_2] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\bn_beta_9_2.h"
};

// final - in: [N][64][8][8]; out: [N][class_size]
const data_t fc_weight[D][CLASS] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\fc_weight.h"
};
const data_t fc_bias[CLASS] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\fc_bias.h"
};
