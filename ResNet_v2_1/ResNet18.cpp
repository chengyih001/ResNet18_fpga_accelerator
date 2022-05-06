#include "ResNet18.h"

// read data from DRAM to 4d BRAM array
void data_scatter_4d(float* input, float* output, 
                    int param_1, int param_2, int param_3, int param_4) 
{
    memcpy(output, input, sizeof(float)*param_1*param_2*param_3*param_4);
}

// read data from DRAM to 2d BRAM array
void data_scatter_2d(float* input, float* output, 
                    int param_1, int param_2) 
{
    memcpy(output, input, sizeof(float)*param_1*param_2);
}

// read data from DRAM to 1d BRAM array
void data_scatter_1d(float* input, float* output, 
                    int param_1, int offset) 
{
    memcpy(output, input + offset, sizeof(float)*param_1);
}

// write data back to DRAM
void data_gather(float* input, float* output,
                    int data_num)
{
    int i;
    for (i=0; i < data_num; i++) {
        output[i] = input[i];
    }
}

float ptr_to_arr4d(float* ptr,
                    int i, int j, int k, int l,
                    int I, int J, int K, int L)
{
    return *(ptr + i*J*K*L + j*K*L + k*L + l);
}

// convolution layer
void conv(float* input, float* output, float* weight, float* bias,
            int wei_fil_0, int wei_fil_1, int wei_h, int wei_w,
            int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w,
            int stride=1, int pad=1)
{
	const int MIN_TRIPCOUNT = N*3*H_1*W_1;
	const int MAX_TRIPCOUNT = N*FIL_1*H_1*W_1;

    int MAX_OUTPUT_BUFFER = N*FIL_1*H_1*W_1;
    int MAX_INPUT_BUFFER = N*FIL_1*H_1*W_1;
    int MAX_WEIGHT_BUFFER = FIL_3*FIL_3*CONV_H*CONV_W;
    int MAX_BIAS_BUFFER = FIL_3;

    float output_buffer[MAX_OUTPUT_BUFFER];
    float input_buffer[MAX_INPUT_BUFFER];
    float weight_buffer[MAX_WEIGHT_BUFFER];
    float bias_buffer[MAX_BIAS_BUFFER];
    data_scatter_4d(input, &input_buffer[0], N, in_fil, in_h, in_w);
    data_scatter_4d(weight, &weight_buffer[0], wei_fil_0, wei_fil_1, wei_h, wei_w);
    data_scatter_1d(bias, &bias_buffer[0], wei_fil_0, 0);

    int MAX_IN_PAD = N*FIL_1*(H_1+2)*(W_1+2);
    int n, c, h, w, f, hh, ww;
    // pad input
    float in_pad[MAX_IN_PAD];
    for (n=0; n < MAX_IN_PAD; n++) {
//#pragma HLS UNROLL
        in_pad[n] = 0;
    }

    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < in_fil; c++) {
            for (h=0; h < in_h; h++) {
                for (w=0; w < in_w; w++) {
#pragma HLS loop_tripcount min=MIN_TRIPCOUNT max=MAX_TRIPCOUNT
#pragma HLS UNROLL
                    in_pad[n*in_fil*(in_h+2*pad)*(in_w+2*pad) + c*(in_h+2*pad)*(in_w+2*pad) + (h+pad)*(in_w+2*pad) + (w+pad)] = ptr_to_arr4d(input, n, c, h, w, N, in_fil, in_h, in_w);
                }
            }
        }
    }

    float temp;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (f=0; f < out_fil; f++) {
            for (h=0; h < out_h; h++) {
                for (w=0; w < out_w; w++) {
                    output_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] = bias_buffer[f];
                    for (c=0; c < in_fil; c++) {
                        for (hh=0; hh < wei_h; hh++) {
                            for (ww=0; ww < wei_w; ww++) {
#pragma HLS UNROLL
                                temp = output_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w];
                                output_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] = temp + ptr_to_arr4d(weight_buffer, f, c, hh, ww, out_fil, in_fil, wei_h, wei_w) * ptr_to_arr4d(in_pad, n, c, h*stride+hh, w*stride+ww, N, in_fil, in_h+2*pad, in_w+2*pad);
                            }
                        }
                    }
                }
            }
        }
    }

    data_gather(&output_buffer[0], output, N*out_fil*out_h*out_w);
}

// batch_normalization layer
void bn(float* input, float* output, float* weight,
            int mean_size, int var_size, int gamma_size, int beta_size,
            int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w,
            float eps=0.00001)
{
	const int MIN_TRIPCOUNT = N*FIL_3*H_3*W_3;
	const int MAX_TRIPCOUNT = N*FIL_1*H_1*W_1;

    int MAX_OUTPUT_BUFFER = N*FIL_1*H_1*W_1;
    int MAX_INPUT_BUFFER = N*FIL_1*H_1*W_1;
    int MAX_MEAN_BUFFER = FIL_3;
    int MAX_VAR_BUFFER = FIL_3;
    int MAX_GAMMA_BUFFER = FIL_3;
    int MAX_BETA_BUFFER = FIL_3;

    float output_buffer[MAX_OUTPUT_BUFFER];
    float input_buffer[MAX_INPUT_BUFFER];
    float mean[MAX_MEAN_BUFFER];
    float var[MAX_VAR_BUFFER];
    float gamma[MAX_GAMMA_BUFFER];
    float beta[MAX_BETA_BUFFER];
    data_scatter_4d(input, &input_buffer[0], N, in_fil, in_h, in_w);
    data_scatter_1d(weight, &mean[0], mean_size, 0);
    data_scatter_1d(weight, &var[0], var_size, mean_size);
    data_scatter_1d(weight, &gamma[0], gamma_size, mean_size+gamma_size);
    data_scatter_1d(weight, &beta[0], beta_size, mean_size+gamma_size+beta_size);
    int n, c, h, w;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < out_fil; c++) {
            for (h=0; h < out_h; h++) {
                for (w=0; w < out_w; w++) {
#pragma HLS loop_tripcount min=MIN_TRIPCOUNT max=MAX_TRIPCOUNT
#pragma HLS UNROLL
                     output_buffer[n*out_fil*out_h*out_w + c*out_h*out_w + h*out_w + w] = gamma[c] * (ptr_to_arr4d(input_buffer, n, c, h, w, N, out_fil, out_h, out_w) - mean[c]) / sqrt((var[c] + eps)) + beta[c];
                }
            }
        }
    }
    
    data_gather(&output_buffer[0], output, N*out_fil*out_h*out_w);
}

// relu layer
void relu(float* input, float* output,
            int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w)
{
#pragma HLS INLINE off
	const int MIN_TRIPCOUNT = N*FIL_3*H_3*W_3;
	const int MAX_TRIPCOUNT = N*FIL_1*H_1*W_1;

    int MAX_OUTPUT_BUFFER = N*FIL_1*H_1*W_1;
    int MAX_INPUT_BUFFER = N*FIL_1*H_1*W_1;
    float output_buffer[MAX_OUTPUT_BUFFER];
    float input_buffer[MAX_INPUT_BUFFER];
    data_scatter_4d(input, &input_buffer[0], N, in_fil, in_h, in_w);
    int n, c, h, w;
    float temp;
    for (n=0; n < N; n++) {
#pragma HLS LOOP_FLATTEN
        for (c=0; c < out_fil; c++) {
            for (h=0; h < out_h; h++) {
                for (w=0; w < out_w; w++) {
#pragma HLS loop_tripcount min=MIN_TRIPCOUNT max=MAX_TRIPCOUNT
#pragma HLS UNROLL
                    temp = ptr_to_arr4d(input_buffer, n, c, h, w, N, out_fil, out_h, out_w);
                    if (temp > 0.0) {
                        output_buffer[n*out_fil*out_h*out_w + c*out_h*out_w + h*out_w + w] = temp;
                    }
                    else {
                        output_buffer[n*out_fil*out_h*out_w + c*out_h*out_w + h*out_w + w] = 0.0;
                    }
                }
            }
        }
    }

    data_gather(&output_buffer[0], output, N*out_fil*out_h*out_w);
}


// input: [N][in_fil][in_h][in_w]; output: [N][out_fil][out_h][out_w]
// layer 0 (conv): param_1 = FIL_X, param_2 = C, param_3 = WEI_H, param_4 = WEI_W
// layer 1 (bn): param_1 = FIL_X, param_2 = FIL_X, param_2 = FIL_X, param_2 = FIL_X
// layer 2 (relu)
void RESNET18_accelerator(float* input,  float* output, float* weight, float* bias, 
                int param_1, int param_2, int param_3, int param_4,
                int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w,
                int stride, int pad,
                int layer)
{
    // layer 0 (conv)
    if (layer==0) {conv(input, output, weight, bias, param_1, param_2, param_3, param_4, in_fil, in_h, in_w, out_fil, out_h, out_w, stride, pad);}

    // layer 1 (bn)
    else if (layer==1) {bn(input, output, weight, param_1, param_2, param_3, param_4, in_fil, in_h, in_w, out_fil, out_h, out_w, 0.00001);}
    
    // layer 2 (relu)
    else if (layer==2) {relu(input, output, in_fil, in_h, in_w, out_fil, out_h, out_w);}

    // // layer 3 (residual) 
    // else if (layer==3) {residual(input, weight, output, in_fil, in_h, in_w, param_2, param_3, param_4, out_fil, out_h, out_w);}

    else {printf("Error passing layer!!!!!");} 

}
