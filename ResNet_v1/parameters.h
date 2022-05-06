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

// final - in: [N][64][8][8]; out: [N][class_size]
const data_t fc_weight[D][CLASS] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\fc_weight.h"
};
const data_t fc_bias[CLASS] = {
    #include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\fc_bias.h"
};
