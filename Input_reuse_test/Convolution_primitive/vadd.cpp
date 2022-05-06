#include "vadd.h"


// convolution layer primitive
void conv(d_type* in_buffer, d_type* out_buffer, d_type* weight_buffer, d_type* bias_buffer,
            int wei_fil_0, int wei_fil_1, int wei_h, int wei_w,
            int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w,
            int stride=1, int pad=1)
{
    const int MAX_IN_PAD = N*FIL_1*(H_1+2)*(W_1+2);
    int n, c, h, w, f, hh, ww;


    // pad input
    d_type in_pad[MAX_IN_PAD];
#pragma HLS array_partition variable=in_pad cyclic factor=9

    for (n=0; n < MAX_IN_PAD; n++) {
#pragma HLS PIPELINE
        in_pad[n] = 0.0;
    }

conv_0_n:
    for (n=0; n < N; n++) {
    conv_0_c:
        for (c=0; c < in_fil; c++) {
        conv_0_h:
            for (h=0; h < in_h; h++) {
            conv_0_w:
                for (w=0; w < in_w; w++) {
#pragma HLS PIPELINE
                    in_pad[n*in_fil*(in_h+2*pad)*(in_w+2*pad) + c*(in_h+2*pad)*(in_w+2*pad) + (h+pad)*(in_w+2*pad) + (w+pad)] = in_buffer[n*in_fil*(in_h)*(in_w) + c*(in_h)*(in_w) + (h)*(in_w) + (w)];
                }
            }
        }
    }

	for (n=0; n < N; n++) {
	conv_1_f:
		for (f=0; f < out_fil; f++) {
		conv_1_h:
			for (h=0; h < out_h; h++) {
			conv_1_w:
				for (w=0; w < out_w; w++) {
#pragma HLS PIPELINE
					out_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] = bias_buffer[f];
				}
			}
		}
	}

	d_type temp[9];
	d_type temp_out[FIL_3];
	d_type weight_local[FIL_1][CONV_H][CONV_W];
#pragma HLS array_partition variable=temp complete
#pragma HLS array_partition variable=temp_out
#pragma HLS array_partition variable=weight_local cyclic dim=1 factor=2
conv_2_n:
    for (n=0; n < N; n++) {
    conv_2_f:
        for (f=0; f < out_fil; f++) {
        conv_2_h:
            for (h=0; h < out_h; h++) {
                conv_2_w:
                for (w=0; w < out_w; w++) {

                    for (c=0; c < in_fil; c++) {
                    conv_2_hh:
                        for (hh=0; hh < 3; hh++) {
                        conv_2_ww:
                            for (ww=0; ww < 3; ww++) {
#pragma HLS PIPELINE
                            	 weight_local[c][hh][ww] = weight_buffer[f*in_fil*wei_h*wei_w + c*wei_h*wei_w + hh*wei_w + ww];
                            }
                        }
                    }

                    for (c=0; c < in_fil; c++) {
#pragma HLS PIPELINE off
                        for (hh=0; hh < 3; hh++) {
#pragma HLS UNROLL
                            for (ww=0; ww < 3; ww++) {
#pragma HLS UNROLL
                            	 temp[hh * wei_w + ww] = weight_local[c][hh][ww] * in_pad[n*in_fil*(in_h+2*pad)*(in_w+2*pad) + c*(in_h+2*pad)*(in_w+2*pad) + (h*stride+hh)*(in_w+2*pad) + (w*stride+ww)];
                            }
                        }
                        d_type sum1 = temp[0] + temp[1];
                        d_type sum2 = temp[2] + temp[3];
                        d_type sum3 = temp[4] + temp[5];
                        d_type sum4 = temp[6] + temp[7];
                        d_type sum5 = sum1 + sum2;
                        d_type sum6 = sum3 + sum4;
                        d_type sum7 = sum5 + sum6;
                        temp_out[c] = sum7 + temp[8];
                    }

                    for (c=0; c < in_fil; c++) {
#pragma HLS PIPELINE off
                        out_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] += temp_out[c];
                    }

                }
            }
        }
    }
}

// // convolution layer
// void conv(d_type* in_buffer, d_type* out_buffer, d_type* weight_buffer, d_type* bias_buffer,
//             int wei_fil_0, int wei_fil_1, int wei_h, int wei_w,
//             int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w,
//             int stride=1, int pad=1)
// {
//     const int MAX_IN_PAD = N*FIL_1*(H_1+2)*(W_1+2);
//     int n, c, h, w, f, hh, ww;


//     // pad input
//     d_type in_pad[MAX_IN_PAD];

//     for (n=0; n < MAX_IN_PAD; n++) {
//         in_pad[n] = 0.0;
//     }

// conv_0_n:
//     for (n=0; n < N; n++) {
//     conv_0_c:
//         for (c=0; c < in_fil; c++) {
//         conv_0_h:
//             for (h=0; h < in_h; h++) {
//             conv_0_w:
//                 for (w=0; w < in_w; w++) {
//                     in_pad[n*in_fil*(in_h+2*pad)*(in_w+2*pad) + c*(in_h+2*pad)*(in_w+2*pad) + (h+pad)*(in_w+2*pad) + (w+pad)] = in_buffer[n*in_fil*(in_h)*(in_w) + c*(in_h)*(in_w) + (h)*(in_w) + (w)];
//                 }
//             }
//         }
//     }

// conv_1_n:
// 	for (n=0; n < N; n++) {
// 	conv_1_f:
// 		for (f=0; f < out_fil; f++) {
// 		conv_1_h:
// 			for (h=0; h < out_h; h++) {
// 			conv_1_w:
// 				for (w=0; w < out_w; w++) {
// 				conv_1_c:
// 					out_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] = bias_buffer[f];
// 				}
// 			}
// 		}
// 	}

// 	d_type temp[9];
// conv_2_n:
//     for (n=0; n < N; n++) {
//     conv_2_f:
//         for (f=0; f < out_fil; f++) {
//         conv_2_h:
//             for (h=0; h < out_h; h++) {
//             conv_2_w:
//                 for (w=0; w < out_w; w++) {
//                 conv_2_c:
//                     for (c=0; c < in_fil; c++) {
//                     conv_2_hh:
//                         for (hh=0; hh < wei_h; hh++) {
//                         conv_2_ww:
//                             for (ww=0; ww < wei_w; ww++) {
//                             	 temp[hh * wei_w + ww] = weight_buffer[f*in_fil*wei_h*wei_w + c*wei_h*wei_w + hh*wei_w + ww] * in_pad[n*in_fil*(in_h+2*pad)*(in_w+2*pad) + c*(in_h+2*pad)*(in_w+2*pad) + (h*stride+hh)*(in_w+2*pad) + (w*stride+ww)];
//                             }
//                         }
//                         d_type sum1 = temp[0] + temp[1];
//                         d_type sum2 = temp[2] + temp[3];
//                         d_type sum3 = temp[4] + temp[5];
//                         d_type sum4 = temp[6] + temp[7];
//                         d_type sum5 = sum1 + sum2;
//                         d_type sum6 = sum3 + sum4;
//                         d_type sum7 = sum5 + sum6;
//                         out_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] = out_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] + sum7 + temp[8];
//                     }
//                 }
//             }
//         }
//     }
// }




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

#pragma HLS array_partition variable=weight_buffer cyclic factor=18

    conv(input_buffer, output_buffer, weight_buffer, bias_buffer, param_1, param_2, param_3, param_4, in_fil, in_h, in_w, out_fil, out_h, out_w, stride, pad);


burst_write_out:
    for (int i = 0; i < OUTPUT_SIZE; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min = 1 max = MAX_OUTPUT_BUFFER
        out[i] = output_buffer[i];
    }

}

}
