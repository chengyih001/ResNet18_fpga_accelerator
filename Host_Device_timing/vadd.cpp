#include "vadd.h"

// convolution layer optimized
void conv_optim(d_type* in_buffer, d_type* out_buffer, d_type* weight_buffer, d_type* bias_buffer,
            int wei_fil_0, int wei_fil_1, int wei_h, int wei_w,
            int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w,
            int stride=1, int pad=1)
{
    const int MAX_IN_PAD = N*FIL_1*(H_1+2)*(W_1+2);

    d_type in_local[CH_SIZE][CONV_H][CONV_W];
    d_type weight_local[CH_SIZE][CONV_H][CONV_W];
    d_type out_local[CH_SIZE][CONV_H][CONV_W];
    d_type in_pad[MAX_IN_PAD];

    #pragma HLS array_partition variable=in_local complete dim=0
    #pragma HLS array_partition variable=weight_local complete dim=0
    #pragma HLS array_partition variable=out_local complete dim=0
    #pragma HLS array_partition variable=in_pad cyclic factor=16


    int c_size = wei_fil_1 < CH_SIZE ? wei_fil_1 : CH_SIZE;

    d_type s_000, s_001, s_002, s_003, s_004, s_005, s_006, s_007, s_008, s_009, s_010, s_011, s_012, s_013, s_014, s_015, s_016, s_017, s_018, s_019, \
           s_020, s_021, s_022, s_023, s_024, s_025, s_026, s_027, s_028, s_029, s_030, s_031, s_032, s_033, s_034, s_035, \
           s_100, s_101, s_102, s_103, s_104, s_105, s_106, s_107, s_108, s_109, s_110, s_111, s_112, s_113, s_114, s_115, s_116, s_117, \
           s_200, s_201, s_202, s_203, s_204, s_205, s_206, s_207, s_208, \
           s_300, s_301, s_302, s_303, s_304, \
           s_400, s_401, s_402, \
           s_500, s_501, \
           s_600;

    // pad input
    for (int n=0; n < MAX_IN_PAD; n++) {
#pragma HLS PIPELINE
        in_pad[n] = 0.0;
    }
conv_0_n:
    for (int n=0; n < N; n++) {
    conv_0_c:
        for (int c=0; c < in_fil; c++) {
        conv_0_h:
            for (int h=0; h < in_h; h++) {
            conv_0_w:
                for (int w=0; w < in_w; w++) {
#pragma HLS PIPELINE
                    in_pad[n*in_fil*(in_h+2*pad)*(in_w+2*pad) + c*(in_h+2*pad)*(in_w+2*pad) + (h+pad)*(in_w+2*pad) + (w+pad)] = in_buffer[n*in_fil*(in_h)*(in_w) + c*(in_h)*(in_w) + (h)*(in_w) + (w)];
                }
            }
        }
    }

    // initialize output
conv_1_n:
	for (int n=0; n < N; n++) {
	conv_1_f:
		for (int f=0; f < out_fil; f++) {
		conv_1_h:
			for (int h=0; h < out_h; h++) {
			conv_1_w:
				for (int w=0; w < out_w; w++) {
#pragma HLS PIPELINE
					out_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] = bias_buffer[f];
				}
			}
		}
	}


    for (int n=0; n < N; n++) {
        for (int h=0; h < out_h; h += 1) {
            for (int w=0; w < out_w; w += 1) {
                for (int c=0; c < in_fil; c += c_size) {

                    // read partial input to local registers
                    for (int i=0; i < c_size; i++) {
                        for (int j=0; j < CONV_H; j++) {
                            for (int k=0; k < CONV_W; k++) {
#pragma HLS PIPELINE
                                in_local[i][j][k] = in_pad[n*in_fil*(in_h+2*pad)*(in_w+2*pad) + (c+i)*(in_h+2*pad)*(in_w+2*pad) + (h*stride+j)*(in_w+2*pad) + (w*stride+k)];
                                // out_local[i][j][k] = out_buffer[n*out_fil*out_h*out_w + (c+i)*out_h*out_w + (h+j)*out_w + (w+k)];
                                out_local[i][j][k] = 0;
                            }
                        }
                    }


                    for (int f=0; f < wei_fil_0; f++) {
#pragma HLS PIPELINE
                        // read partial weight to local registers
                        for (int i=0; i < c_size; i++) {
                            for (int j=0; j < CONV_H; j++) {
                                for (int k=0; k < CONV_W; k++) {
#pragma HLS PIPELINE
                                    weight_local[i][j][k] = weight_buffer[f*wei_fil_1*wei_h*wei_w + (c+i)*wei_h*wei_w + j*wei_w + k];
                                }
                            }
                        }

                        for (int i=0; i < c_size; i++) {
//                            for (int j=0; j < CONV_H; j++) {
//#pragma HLS unroll
//                                for (int k=0; k < CONV_W; k++) {
//#pragma HLS unroll
//                                    out_local[i][j][k] = in_local[i][j][k] * weight_local[i][j][k];
//                                }
//                            }

#pragma HLS PIPELINE
                            out_local[i][0][0] = in_local[i][0][0] * weight_local[i][0][0];
                            out_local[i][0][1] = in_local[i][0][1] * weight_local[i][0][1];
                            out_local[i][0][2] = in_local[i][0][2] * weight_local[i][0][2];
                            out_local[i][1][0] = in_local[i][1][0] * weight_local[i][1][0];
                            out_local[i][1][1] = in_local[i][1][1] * weight_local[i][1][1];
                            out_local[i][1][2] = in_local[i][1][2] * weight_local[i][1][2];
                            out_local[i][2][0] = in_local[i][2][0] * weight_local[i][2][0];
                            out_local[i][2][1] = in_local[i][2][1] * weight_local[i][2][1];
                            out_local[i][2][2] = in_local[i][2][2] * weight_local[i][2][2];
                        }


                        s_000 = out_local[0][0][0] + out_local[0][0][1];
                        s_001 = out_local[0][0][2] + out_local[0][1][0];
                        s_002 = out_local[0][1][1] + out_local[0][1][2];
                        s_003 = out_local[0][2][0] + out_local[0][2][1];
                        s_004 = out_local[0][2][2] + out_local[1][0][0];
                        s_005 = out_local[1][0][1] + out_local[1][0][2];
                        s_006 = out_local[1][1][0] + out_local[1][1][1];
                        s_007 = out_local[1][1][2] + out_local[1][2][0];
                        s_008 = out_local[1][2][1] + out_local[1][2][2];

                        s_009 = out_local[2][0][0] + out_local[2][0][1];
                        s_010 = out_local[2][0][2] + out_local[2][1][0];
                        s_011 = out_local[2][1][1] + out_local[2][1][2];
                        s_012 = out_local[2][2][0] + out_local[2][2][1];
                        s_013 = out_local[2][2][2] + out_local[3][0][0];
                        s_014 = out_local[3][0][1] + out_local[3][0][2];
                        s_015 = out_local[3][1][0] + out_local[3][1][1];
                        s_016 = out_local[3][1][2] + out_local[3][2][0];
                        s_017 = out_local[3][2][1] + out_local[3][2][2];

                        s_018 = out_local[4][0][0] + out_local[4][0][1];
                        s_019 = out_local[4][0][2] + out_local[4][1][0];
                        s_020 = out_local[4][1][1] + out_local[4][1][2];
                        s_021 = out_local[4][2][0] + out_local[4][2][1];
                        s_022 = out_local[4][2][2] + out_local[5][0][0];
                        s_023 = out_local[5][0][1] + out_local[5][0][2];
                        s_024 = out_local[5][1][0] + out_local[5][1][1];
                        s_025 = out_local[5][1][2] + out_local[5][2][0];
                        s_026 = out_local[5][2][1] + out_local[5][2][2];

                        s_027 = out_local[6][0][0] + out_local[6][0][1];
                        s_028 = out_local[6][0][2] + out_local[6][1][0];
                        s_029 = out_local[6][1][1] + out_local[6][1][2];
                        s_030 = out_local[6][2][0] + out_local[6][2][1];
                        s_031 = out_local[6][2][2] + out_local[7][0][0];
                        s_032 = out_local[7][0][1] + out_local[7][0][2];
                        s_033 = out_local[7][1][0] + out_local[7][1][1];
                        s_034 = out_local[7][1][2] + out_local[7][2][0];
                        s_035 = out_local[7][2][1] + out_local[7][2][2];

                        s_100 = s_000 + s_001;
                        s_101 = s_002 + s_003;
                        s_102 = s_004 + s_005;
                        s_103 = s_006 + s_007;
                        s_104 = s_008 + s_009;
                        s_105 = s_010 + s_011;
                        s_106 = s_012 + s_013;
                        s_107 = s_014 + s_015;
                        s_108 = s_016 + s_017;

                        s_109 = s_018 + s_019;
                        s_110 = s_020 + s_021;
                        s_111 = s_022 + s_023;
                        s_112 = s_024 + s_025;
                        s_113 = s_026 + s_027;
                        s_114 = s_028 + s_029;
                        s_115 = s_030 + s_031;
                        s_116 = s_032 + s_033;
                        s_117 = s_034 + s_035;

                        s_200 = s_100 + s_101;
                        s_201 = s_102 + s_103;
                        s_202 = s_104 + s_105;
                        s_203 = s_106 + s_107;
                        s_204 = s_108 + s_109;
                        s_205 = s_110 + s_111;
                        s_206 = s_112 + s_113;
                        s_207 = s_114 + s_115;
                        s_208 = s_116 + s_117;

                        s_300 = s_200 + s_201;
                        s_301 = s_202 + s_203;
                        s_302 = s_204 + s_205;
                        s_303 = s_206 + s_207;
                        s_304 = s_208;

                        s_400 = s_300 + s_301;
                        s_401 = s_302 + s_303;
                        s_402 = s_304;

                        s_500 = s_400 + s_401;
                        s_501 = s_402;

                        s_600 = s_500 + s_501;

                        out_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] += s_600;

                    }
                }
            }
        }
    }
}

// batch_normalization layer
void bn(d_type* in_buffer, d_type* out_buffer, d_type* weight_buffer,
            int mean_size, int var_size, int gamma_size, int beta_size,
            int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w,
            float eps=0.00001)
{
    d_type* gamma_buffer = weight_buffer;
    d_type* beta_buffer = &weight_buffer[mean_size];
    d_type* mean_buffer = &weight_buffer[mean_size+var_size];
    d_type* var_buffer = &weight_buffer[mean_size+var_size+gamma_size];

    int n, c, h, w;
    for (n=0; n < N; n++) {
        for (c=0; c < out_fil; c++) {
            for (h=0; h < out_h; h++) {
                for (w=0; w < out_w; w++) {
#pragma HLS PIPELINE
                    out_buffer[n*out_fil*out_h*out_w + c*out_h*out_w + h*out_w + w] = gamma_buffer[c] * (in_buffer[n*out_fil*out_h*out_w + c*out_h*out_w + h*out_w + w] - mean_buffer[c]) / sqrt((var_buffer[c] + eps)) + beta_buffer[c];
                }
            }
        }
    }
}


// relu layer
void relu(float* in_buffer, float* out_buffer,
            int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w)
{
    int n, c, h, w;
    for (n=0; n < N; n++) {
        for (c=0; c < out_fil; c++) {
            for (h=0; h < out_h; h++) {
                for (w=0; w < out_w; w++) {
#pragma HLS PIPELINE
                    if (in_buffer[n*out_fil*out_h*out_w + c*out_h*out_w + h*out_w + w] < 0.0) {
                        out_buffer[n*out_fil*out_h*out_w + c*out_h*out_w + h*out_w + w] = 0.0;
                    }
                    else {
                        out_buffer[n*out_fil*out_h*out_w + c*out_h*out_w + h*out_w + w] = in_buffer[n*out_fil*out_h*out_w + c*out_h*out_w + h*out_w + w];
                    }
                }
            }
        }
    }
}

// residual
void residual(float* in_buffer, float* out_buffer, float* residual_buffer,
                int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w)
{
    int r_count = N*out_fil*out_h*out_w;
    for (int i=0; i < r_count; i += CH_SIZE) {
#pragma HLS PIPELINE
        for (int j=0; j < CH_SIZE; j++) {
            out_buffer[i+j] = in_buffer[i+j] + residual_buffer[i+j];
        }
    }

}

extern "C" {

// input: [N][in_fil][in_h][in_w]; output: [N][out_fil][out_h][out_w]
// layer 0 (conv): param_1 = FIL_X, param_2 = C, param_3 = WEI_H, param_4 = WEI_W
// layer 1 (bn): param_1 = FIL_X, param_2 = FIL_X, param_2 = FIL_X, param_2 = FIL_X
// layer 2 (relu)
void vadd(d_type* in, d_type* out, d_type* weight, d_type* bias,
                int param_1, int param_2, int param_3, int param_4,
                int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w,
                int stride, int pad,
                int layer, int residual_save)
{
// Map pointers to AXI4-master interface for global memory access
#pragma HLS INTERFACE m_axi port = in offset = slave bundle = GMEM0 max_read_burst_length = 256 max_write_burst_length = 256
#pragma HLS INTERFACE m_axi port = out offset = slave bundle = GMEM0 max_read_burst_length = 256 max_write_burst_length = 256
#pragma HLS INTERFACE m_axi port = weight offset = slave bundle = GMEM1 max_read_burst_length = 256
#pragma HLS INTERFACE m_axi port = bias offset = slave bundle = GMEM2 max_read_burst_length = 256

// Map pointers and return to a bundled axilite slave interface
#pragma HLS INTERFACE s_axilite register port = return bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = in bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = out bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = weight bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = bias bundle = CTRL_SIGNAL

#pragma HLS INTERFACE s_axilite register port = param_1 bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = param_2 bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = param_3 bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = param_4 bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = in_fil bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = in_h bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = in_w bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = out_fil bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = out_h bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = out_w bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = stride bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = pad bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = layer bundle = CTRL_SIGNAL
#pragma HLS INTERFACE s_axilite register port = residual_save bundle = CTRL_SIGNAL
#pragma HLS INTERFACE ap_ctrl_chain register port = return


    static const int MAX_INPUT_BUFFER = N*FIL_1*H_1*W_1;
    static const int MAX_OUTPUT_BUFFER = N*FIL_1*H_1*W_1;

    static d_type output_buffer[MAX_OUTPUT_BUFFER];

    const int INPUT_SIZE = N*in_fil*in_h*in_w;
    const int OUTPUT_SIZE = N*out_fil*out_h*out_w;


// layer 0: conv
    if (layer == 0) {

        conv_optim(in, output_buffer, weight, bias, param_1, param_2, param_3, param_4, in_fil, in_h, in_w, out_fil, out_h, out_w, stride, pad);

    }

// layer 1: bn
    else if (layer == 1) {
    	d_type input_buffer[MAX_INPUT_BUFFER];

burst_read_input_bn:
        for (int i = 0; i < INPUT_SIZE; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = MAX_INPUT_BUFFER
            input_buffer[i] = in[i];
        }

        const int MAX_MEAN_BUFFER = FIL_3;
        const int MAX_VAR_BUFFER = FIL_3;
        const int MAX_GAMMA_BUFFER = FIL_3;
        const int MAX_BETA_BUFFER = FIL_3;

        const int MEAN_SIZE = param_1;
        const int VAR_SIZE = param_2;
        const int GAMMA_SIZE =  param_3;
        const int BETA_SIZE =  param_4;

        d_type weight_buffer[MAX_MEAN_BUFFER + MAX_VAR_BUFFER + MAX_GAMMA_BUFFER + MAX_BETA_BUFFER];

#pragma HLS array_partition variable=weight_buffer block factor=4

burst_read_weights_bn:
        for (int i = 0; i < MEAN_SIZE + VAR_SIZE + GAMMA_SIZE + BETA_SIZE; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = MAX_MEAN_BUFFER
            weight_buffer[i] = weight[i];
        }

        bn(input_buffer, output_buffer, weight_buffer, param_1, param_2, param_3, param_4, in_fil, in_h, in_w, out_fil, out_h, out_w);

    }

// layer 2: relu
    else if (layer == 2){
    	d_type input_buffer[MAX_INPUT_BUFFER];

burst_read_input_relu:
        for (int i = 0; i < INPUT_SIZE; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = MAX_INPUT_BUFFER
            input_buffer[i] = in[i];
        }
        relu(input_buffer, output_buffer, in_fil, in_h, in_w, out_fil, out_h, out_w);

        if (residual_save == 1) {
burst_write_residual:
            for (int i = 0; i < OUTPUT_SIZE; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = MAX_OUTPUT_BUFFER
                weight[i] = output_buffer[i];
            }
        }
    }

// layer 3: residual
    else {
        const int MAX_RESIDUAL_BUFFER = N*FIL_1*H_1*W_1;

        const int RESIDUAL_SIZE = INPUT_SIZE;

        d_type residual_buffer[MAX_RESIDUAL_BUFFER];
        d_type input_buffer[MAX_INPUT_BUFFER];

#pragma HLS array_partition variable=output_buffer cyclic factor=8
#pragma HLS array_partition variable=input_buffer cyclic factor=8
#pragma HLS array_partition variable=residual_buffer cyclic factor=8

burst_read_input_residual:
		for (int i = 0; i < INPUT_SIZE; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = MAX_INPUT_BUFFER
			input_buffer[i] = in[i];
		}
burst_read_residual:
        for (int i = 0; i < INPUT_SIZE; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = MAX_INPUT_BUFFER
            residual_buffer[i] = weight[i];
        }


        residual(input_buffer, output_buffer, residual_buffer, in_fil, in_h, in_w, out_fil, out_h, out_w);

    }

burst_write_out:
    for (int i = 0; i < OUTPUT_SIZE; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = MAX_OUTPUT_BUFFER
        out[i] = output_buffer[i];
    }

}

}
