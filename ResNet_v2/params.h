#ifndef PARAMS_H
#define PARAMS_H

#pragma once

//#include "ap_fixed.h"
// #include "hls_math.h"

//typedef ap_fixed<16,4,AP_RND,AP_SAT> data_t;
 typedef float data_t;

#define N 1
#define C 3

#define H_0 32
#define W_0 32
#define H_1 16
#define W_1 16
#define H_2 8
#define W_2 8

#define FIL_0 16
#define FIL_1 32
#define FIL_2 64

#define WEI_H 3
#define WEI_W 3

#define H_0_PAD 34
#define W_0_PAD 34
#define H_1_PAD 18
#define W_1_PAD 18
#define H_2_PAD 10
#define W_2_PAD 10

#define STR_0 1
#define STR_1 2
#define PAD_0 1

#define FC_H_0 1
#define FC_W_0 1
#define FC_FIL_0 64

#define D FC_FIL_0*FC_H_0*FC_W_0

#define CLASS 10


#define FIL 64
#define IN_H 8
#define IN_W 8

#define OUT_H 1
#define OUT_W 1

#define PAD false

#endif
