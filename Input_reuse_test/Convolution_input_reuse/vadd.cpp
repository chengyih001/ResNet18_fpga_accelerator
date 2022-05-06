#include "vadd.h"

// convolution layer optimized
void conv_optim(d_type* in_buffer, d_type* out_buffer, d_type* weight_buffer, d_type* bias_buffer,
            int wei_fil_0, int wei_fil_1, int wei_h, int wei_w,
            int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w,
            int stride=1, int pad=1)
{
    const int MAX_IN_PAD = N*FIL_1*(H_1+2)*(W_1+2);

    d_type in_local[CH_SIZE][CONV_H][CONV_W];
    d_type weight_local[FIL_3][CH_SIZE][CONV_H][CONV_W];
    d_type out_local[CH_SIZE][CONV_H][CONV_W];
    d_type in_pad[MAX_IN_PAD];

    int c_size = wei_fil_1 < CH_SIZE ? wei_fil_1 : CH_SIZE;

    #pragma HLS array_partition variable=in_local complete dim=0
    #pragma HLS array_partition variable=weight_local complete dim=2
	#pragma HLS array_partition variable=weight_local complete dim=3
	#pragma HLS array_partition variable=weight_local complete dim=4
    #pragma HLS array_partition variable=out_local complete dim=0
    #pragma HLS array_partition variable=in_pad cyclic factor=16


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

	d_type temp_out[FIL_3];
	d_type temp_out2[FIL_3];
#pragma HLS array_partition variable=temp_out complete
#pragma HLS array_partition variable=temp_out2 complete

    for (int n=0; n < N; n++) {
        for (int h=0; h < out_h; h += 1) {
            for (int w=0; w < out_w; w += 1) {
                for (int c=0; c < in_fil; c += CH_SIZE) {
//#pragma HLS LOOP_FLATTEN off
//#pragma HLS PIPELINE

                    // read partial input to local registers
                    for (int i=0; i < CH_SIZE; i++) {
                        for (int j=0; j < CONV_H; j++) {
                            for (int k=0; k < CONV_W; k++) {
#pragma HLS PIPELINE
                                in_local[i][j][k] = in_pad[n*in_fil*(in_h+2*pad)*(in_w+2*pad) + (c+i)*(in_h+2*pad)*(in_w+2*pad) + (h*stride+j)*(in_w+2*pad) + (w*stride+k)];
                                // out_local[i][j][k] = out_buffer[n*out_fil*out_h*out_w + (c+i)*out_h*out_w + (h+j)*out_w + (w+k)];
                                out_local[i][j][k] = 0;
                            }
                        }
                    }


                    for (int i=0; i < FIL_3; i++) {
#pragma HLS unroll
                    	temp_out[i] = 0.0;
                    }

                    for (int f=0; f < wei_fil_0; f++) {
#pragma HLS PIPELINE
                    	temp_out2[f] = out_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w];
                    }

                    for (int f=0; f < wei_fil_0; f++) {
                        // read partial weight to local registers
                        for (int i=0; i < CH_SIZE; i++) {
                            for (int j=0; j < CONV_H; j++) {
                                for (int k=0; k < CONV_W; k++) {
#pragma HLS PIPELINE
                                    weight_local[f][i][j][k] = weight_buffer[f*wei_fil_1*wei_h*wei_w + (c+i)*wei_h*wei_w + j*wei_w + k];
                                }
                            }
                        }
                    }

                    for (int f=0; f < wei_fil_0; f++) {
#pragma HLS PIPELINE

                        for (int i=0; i < CH_SIZE; i++) {
#pragma HLS unroll
                            for (int j=0; j < CONV_H; j++) {
#pragma HLS unroll
                                for (int k=0; k < CONV_W; k++) {
#pragma HLS unroll
                                    out_local[i][j][k] = in_local[i][j][k] * weight_local[f][i][j][k];
                                }
                            }

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

                        temp_out[f] = s_500 + s_501;

//                        out_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] += s_600;

                    }

                    for (int f=0; f < wei_fil_0; f++) {
#pragma HLS PIPELINE
                    	out_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] = temp_out[f] + temp_out2[f];
                    }
                }
            }
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


    const int MAX_INPUT_BUFFER = N*FIL_1*H_1*W_1;
    const int MAX_OUTPUT_BUFFER = N*FIL_1*H_1*W_1;

    d_type output_buffer[MAX_OUTPUT_BUFFER];

    const int INPUT_SIZE = N*in_fil*in_h*in_w;
    const int OUTPUT_SIZE = N*out_fil*out_h*out_w;
    d_type input_buffer[MAX_INPUT_BUFFER];

burst_read_input:
    for (int i = 0; i < INPUT_SIZE; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = MAX_INPUT_BUFFER
        input_buffer[i] = in[i];
    }

// layer 0: conv
    const int MAX_WEIGHT_BUFFER = FIL_3*FIL_3*CONV_H*CONV_W;
    const int MAX_BIAS_BUFFER = FIL_3;

    const int WEIGHT_SIZE = param_1*param_2*param_3*param_4;
    const int BIAS_SIZE = param_1;

    d_type weight_buffer[MAX_WEIGHT_BUFFER];
    d_type bias_buffer[MAX_BIAS_BUFFER];

burst_read_weight_conv:
    for (int i = 0; i < WEIGHT_SIZE; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = MAX_WEIGHT_BUFFER
        weight_buffer[i] = weight[i];
    }
burst_read_bias_conv:
    for (int i = 0; i < BIAS_SIZE; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = MAX_BIAS_BUFFER
        bias_buffer[i] = bias[i];
    }

#pragma HLS array_partition variable=weight_buffer cyclic factor=9

    conv_optim(input_buffer, output_buffer, weight_buffer, bias_buffer, param_1, param_2, param_3, param_4, in_fil, in_h, in_w, out_fil, out_h, out_w, stride, pad);


burst_write_out:
    for (int i = 0; i < OUTPUT_SIZE; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = MAX_OUTPUT_BUFFER
        out[i] = output_buffer[i];
    }

}

}
