#include "ResNet18_ps.h"

int get_file_size(const char* filename)
{
    struct stat buf;
    if(stat(filename, &buf)<0)
    {
        return 0;
    }
    return (int)buf.st_size;
}

float* load_data(const char* filename)
{
    int data_num = get_file_size(filename)/4;
	float* data_buf = (float *)calloc(data_num, sizeof(float));
	FILE *fp_w = fopen(filename, "rb");
	fread(data_buf, sizeof(float), data_num, fp_w);
	fclose(fp_w);
    // printf("data size of %s is %d\n", filename, data_num*4);
    return data_buf;
}

void residual(float* input_0, float* input_1, float* output,
                int in_0_fil, int in_0_h, int in_0_w, 
                int in_1_fil, int in_1_h, int in_1_w, 
                int out_fil, int out_h, int out_w)
{
    float input_0_buffer[N][in_0_fil][in_0_h][in_0_w];
    float input_1_buffer[N][in_1_fil][in_1_h][in_1_w];
    float output_buffer[N][out_fil][out_h][out_w];
    data_scatter_4d(input_0, &input_0_buffer[0][0][0][0], N, in_0_fil, in_0_h, in_0_w);
    data_scatter_4d(input_1, &input_1_buffer[0][0][0][0], N, in_1_fil, in_1_h, in_1_w);
    
    int n, c, h, w;
    for (n=0; n < N; n++) {
        for (c=0; c < out_fil; c++) {
            for (h=0; h < out_h; h++) {
                for (w=0; w < out_w; w++) {
                    output_buffer[n][c][h][w] = input_0_buffer[n][c][h][w] + input_1_buffer[n][c][h][w];
                }
            }
        }
    }

    data_gather(&output_buffer[0][0][0][0], output, N*out_fil*out_h*out_w);
}

void gap(float* input, float* output,
            int in_fil, int in_h, int in_w,
            int out_fil, int out_h, int out_w)
{
    float output_buffer[N][out_fil][out_h][out_w];
    float input_buffer[N][in_fil][in_h][in_w];
    data_scatter_4d(input, &input_buffer[0][0][0][0], N, in_fil, in_h, in_w);

    int n, c, h, w;
    for (n=0; n < N; n++) {
        for (c=0; c < out_fil; c++) {
            output_buffer[n][c][0][0] = 0;
        }
    }
    for (n=0; n < N; n++) {
        for (c=0; c < out_fil; c++) {
            for (h=0; h < out_h; h++) {
                for (w=0; w < out_w; w++) {
                    output_buffer[n][c][0][0] = output_buffer[n][c][0][0] + input_buffer[n][c][h][w] / (in_h * in_w);
                }
            }
        }
    }

    data_gather(&output_buffer[0][0][0][0], output, N*out_fil*out_h*out_w);
}

void full_connected(float* input, float* output, float* weight, float* bias,
                        int in_fil, int in_h, int in_w,
                        int out_size,
                        int wei_h, int wei_w,
                        int bias_size)
{
    float output_buffer[N][out_size];
    float input_buffer[N][in_fil][in_h][in_w];
    float weight_buffer[wei_h][wei_w];
    float bias_buffer[bias_size];
    data_scatter_4d(input, &input_buffer[0][0][0][0], N, in_fil, in_h, in_w);
    data_scatter_2d(weight, &weight_buffer[0][0], wei_h, wei_w);
    data_scatter_1d(bias, &bias_buffer[0], bias_size, 0);

    float temp_buffer_0[N][in_fil];

    int n, c, h, w, d, m;
    for (n=0; n < N; n++) {
        for (m=0; m < out_size; m++) {
            output_buffer[n][m] = bias_buffer[m];
        }
    }

    // matrix reshape: [N][64][1][1] -> [N][64]
    for (n=0; n < N; n++) {
        for (c=0; c < in_fil; c++) {
            for (h=0; h < in_h; h++) {
                for (w=0; w < in_w; w++) {
                    temp_buffer_0[n][c * in_h * in_w + h * in_w + w] = input_buffer[n][c][h][w];
                }
            }
        }
    }

    // matrix multipllication: [N][64] x [64][10] -> [N][10]
    for (n=0; n < N; n++) {
        for (m=0; m < out_size; m++) {
            for (d=0; d < in_fil; d++) {
                output_buffer[n][m] = output_buffer[n][m] + temp_buffer_0[n][d] * weight_buffer[d][m];
            }
        }
    }

    data_gather(&output_buffer[0][0], output, N*out_size);
}

void ResNet18_network(float* in, float* out)
{
    int MAX_OUTPUT_SIZE = N*FIL_1*H_1*W_1;

    float* output = (float *)calloc(MAX_OUTPUT_SIZE, sizeof(float));
    float* input;
    float* weight;
    float* bias;

    float* residual_input = (float *)calloc(MAX_OUTPUT_SIZE, sizeof(float));
    // layer 0
    input = in;
    weight = load_data("parameters/conv_weight_0.bin");
    bias = load_data("parameters/conv_bias_0.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_1, 3, CONV_H, CONV_W, 
                            3, 32, 32, FIL_1, H_1, W_1, 
                            1, 1,
                            0);
    // printf("%f \n", output[0]);
    free(weight);
    free(bias);

    input = output;
    weight = load_data("parameters/bn_weight_0.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_1, FIL_1, FIL_1, FIL_1,
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1,
                            _, _,
                            1);

    free(weight);

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1,
                            _, _,
                            2);
    

    // layer 1
    input = output;
    weight = load_data("parameters/conv_weight_1_1.bin");
    bias = load_data("parameters/conv_bias_1_1.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_1, FIL_1, CONV_H, CONV_W, 
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);

    input = output;
    weight = load_data("parameters/bn_weight_1_1.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_1, FIL_1, FIL_1, FIL_1,
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1,
                            _, _,
                            1);

    data_gather(output, residual_input, N*FIL_1*H_1*W_1);
    
    free(weight);

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1,
                            _, _,
                            2);
    

    input = output;
    weight = load_data("parameters/conv_weight_1_2.bin");
    bias = load_data("parameters/conv_bias_1_2.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_1, FIL_1, CONV_H, CONV_W, 
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);
                            
    input = output;
    weight = load_data("parameters/bn_weight_1_2.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_1, FIL_1, FIL_1, FIL_1,
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1,
                            _, _,
                            1);
    
    free(weight);

    input = output;
    residual(residual_input, input, output,
                FIL_1, H_1, W_1,
                FIL_1, H_1, W_1,
                FIL_1, H_1, W_1);
    
    

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1,
                            _, _,
                            2);
    

    // layer 2
    input = output;
    weight = load_data("parameters/conv_weight_2_1.bin");
    bias = load_data("parameters/conv_bias_2_1.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_1, FIL_1, CONV_H, CONV_W, 
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);

    input = output;
    weight = load_data("parameters/bn_weight_2_1.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_1, FIL_1, FIL_1, FIL_1,
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1,
                            _, _,
                            1);
    data_gather(output, residual_input, N*FIL_1*H_1*W_1);
    
    free(weight);

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1,
                            _, _,
                            2);
    

    input = output;
    weight = load_data("parameters/conv_weight_2_2.bin");
    bias = load_data("parameters/conv_bias_2_2.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_1, FIL_1, CONV_H, CONV_W, 
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);
                            
    input = output;
    weight = load_data("parameters/bn_weight_2_2.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_1, FIL_1, FIL_1, FIL_1,
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1,
                            _, _,
                            1);
    
    free(weight);

    input = output;
    residual(residual_input, input, output,
                FIL_1, H_1, W_1,
                FIL_1, H_1, W_1,
                FIL_1, H_1, W_1);

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_1, H_1, W_1, FIL_1, H_1, W_1,
                            _, _,
                            2);
    

    // layer 3
    input = output;
    weight = load_data("parameters/conv_weight_3_1.bin");
    bias = load_data("parameters/conv_bias_3_1.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_2, FIL_1, CONV_H, CONV_W, 
                            FIL_1, H_1, W_1, FIL_2, H_2, W_2, 
                            2, 1,
                            0);
    
    free(weight);
    free(bias);

    input = output;
    weight = load_data("parameters/bn_weight_3_1.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_2, FIL_2, FIL_2, FIL_2,
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2,
                            _, _,
                            1);
    data_gather(output, residual_input, N*FIL_2*H_2*W_2);
    
    free(weight);

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2,
                            _, _,
                            2);
    

    input = output;
    weight = load_data("parameters/conv_weight_3_2.bin");
    bias = load_data("parameters/conv_bias_3_2.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_2, FIL_2, CONV_H, CONV_W, 
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);
                            
    input = output;
    weight = load_data("parameters/bn_weight_3_2.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_2, FIL_2, FIL_2, FIL_2,
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2,
                            _, _,
                            1);
    
    free(weight);

    input = output;
    residual(residual_input, input, output,
                FIL_2, H_2, W_2,
                FIL_2, H_2, W_2,
                FIL_2, H_2, W_2);
    
    

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2,
                            _, _,
                            2);
    

    // layer 4
    input = output;
    weight = load_data("parameters/conv_weight_4_1.bin");
    bias = load_data("parameters/conv_bias_4_1.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_2, FIL_2, CONV_H, CONV_W, 
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);

    input = output;
    weight = load_data("parameters/bn_weight_4_1.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_2, FIL_2, FIL_2, FIL_2,
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2,
                            _, _,
                            1);
    data_gather(output, residual_input, N*FIL_2*H_2*W_2);
    
    free(weight);

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2,
                            _, _,
                            2);
    

    input = output;
    weight = load_data("parameters/conv_weight_4_2.bin");
    bias = load_data("parameters/conv_bias_4_2.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_2, FIL_2, CONV_H, CONV_W, 
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);
                            
    input = output;
    weight = load_data("parameters/bn_weight_4_2.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_2, FIL_2, FIL_2, FIL_2,
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2,
                            _, _,
                            1);
    
    free(weight);

    input = output;
    residual(residual_input, input, output,
                FIL_2, H_2, W_2,
                FIL_2, H_2, W_2,
                FIL_2, H_2, W_2);
    
    

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2,
                            _, _,
                            2);
    

    // layer 5
    input = output;
    weight = load_data("parameters/conv_weight_5_1.bin");
    bias = load_data("parameters/conv_bias_5_1.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_2, FIL_2, CONV_H, CONV_W, 
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);

    input = output;
    weight = load_data("parameters/bn_weight_5_1.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_2, FIL_2, FIL_2, FIL_2,
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2,
                            _, _,
                            1);
    data_gather(output, residual_input, N*FIL_2*H_2*W_2);
    
    free(weight);

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2,
                            _, _,
                            2);
    

    input = output;
    weight = load_data("parameters/conv_weight_5_2.bin");
    bias = load_data("parameters/conv_bias_5_2.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_2, FIL_2, CONV_H, CONV_W, 
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);
                            
    input = output;
    weight = load_data("parameters/bn_weight_5_2.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_2, FIL_2, FIL_2, FIL_2,
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2,
                            _, _,
                            1);
    
    free(weight);

    input = output;
    residual(residual_input, input, output,
                FIL_2, H_2, W_2,
                FIL_2, H_2, W_2,
                FIL_2, H_2, W_2);
    
    

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_2, H_2, W_2, FIL_2, H_2, W_2,
                            _, _,
                            2);
    

    // layer 6
    input = output;
    weight = load_data("parameters/conv_weight_6_1.bin");
    bias = load_data("parameters/conv_bias_6_1.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_3, FIL_2, CONV_H, CONV_W, 
                            FIL_2, H_2, W_2, FIL_3, H_3, W_3, 
                            2, 1,
                            0);
    
    free(weight);
    free(bias);

    input = output;
    weight = load_data("parameters/bn_weight_6_1.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_3, FIL_3, FIL_3, FIL_3,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            1);
    data_gather(output, residual_input, N*FIL_3*H_3*W_3);
    
    free(weight);

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            2);
    

    input = output;
    weight = load_data("parameters/conv_weight_6_2.bin");
    bias = load_data("parameters/conv_bias_6_2.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_3, FIL_3, CONV_H, CONV_W, 
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);
                            
    input = output;
    weight = load_data("parameters/bn_weight_6_2.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_3, FIL_3, FIL_3, FIL_3,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            1);
    
    free(weight);

    input = output;
    residual(residual_input, input, output,
                FIL_3, H_3, W_3,
                FIL_3, H_3, W_3,
                FIL_3, H_3, W_3);
    
    

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            2);
    

    // layer 7
    input = output;
    weight = load_data("parameters/conv_weight_7_1.bin");
    bias = load_data("parameters/conv_bias_7_1.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_3, FIL_3, CONV_H, CONV_W, 
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);

    input = output;
    weight = load_data("parameters/bn_weight_7_1.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_3, FIL_3, FIL_3, FIL_3,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            1);
    data_gather(output, residual_input, N*FIL_3*H_3*W_3);
    
    free(weight);

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            2);
    

    input = output;
    weight = load_data("parameters/conv_weight_7_2.bin");
    bias = load_data("parameters/conv_bias_7_2.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_3, FIL_3, CONV_H, CONV_W, 
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);
                            
    input = output;
    weight = load_data("parameters/bn_weight_7_2.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_3, FIL_3, FIL_3, FIL_3,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            1);
    
    free(weight);

    input = output;
    residual(residual_input, input, output,
                FIL_3, H_3, W_3,
                FIL_3, H_3, W_3,
                FIL_3, H_3, W_3);
    
    

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            2);
    

    // layer 8
    input = output;
    weight = load_data("parameters/conv_weight_8_1.bin");
    bias = load_data("parameters/conv_bias_8_1.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_3, FIL_3, CONV_H, CONV_W, 
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);

    input = output;
    weight = load_data("parameters/bn_weight_8_1.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_3, FIL_3, FIL_3, FIL_3,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            1);
    data_gather(output, residual_input, N*FIL_3*H_3*W_3);
    
    free(weight);

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            2);
    

    input = output;
    weight = load_data("parameters/conv_weight_8_2.bin");
    bias = load_data("parameters/conv_bias_8_2.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_3, FIL_3, CONV_H, CONV_W, 
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);
                            
    input = output;
    weight = load_data("parameters/bn_weight_8_2.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_3, FIL_3, FIL_3, FIL_3,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            1);
    
    free(weight);

    input = output;
    residual(residual_input, input, output,
                FIL_3, H_3, W_3,
                FIL_3, H_3, W_3,
                FIL_3, H_3, W_3);
    
    

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            2);
    

    // layer 9
    input = output;
    weight = load_data("parameters/conv_weight_9_1.bin");
    bias = load_data("parameters/conv_bias_9_1.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_3, FIL_3, CONV_H, CONV_W, 
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);

    input = output;
    weight = load_data("parameters/bn_weight_9_1.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_3, FIL_3, FIL_3, FIL_3,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            1);
    data_gather(output, residual_input, N*FIL_3*H_3*W_3);
    
    free(weight);

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            2);
    

    input = output;
    weight = load_data("parameters/conv_weight_9_2.bin");
    bias = load_data("parameters/conv_bias_9_2.bin");
    RESNET18_accelerator(input, output, weight, bias, 
                            FIL_3, FIL_3, CONV_H, CONV_W, 
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3, 
                            1, 1,
                            0);
    
    free(weight);
    free(bias);
                            
    input = output;
    weight = load_data("parameters/bn_weight_9_2.bin");
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            FIL_3, FIL_3, FIL_3, FIL_3,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            1);
    
    free(weight);

    input = output;
    residual(residual_input, input, output,
                FIL_3, H_3, W_3,
                FIL_3, H_3, W_3,
                FIL_3, H_3, W_3);
    
    

    input = output;
    weight = nullptr;
    bias = nullptr;
    RESNET18_accelerator(input, output, weight, bias,
                            _, _, _, _,
                            FIL_3, H_3, W_3, FIL_3, H_3, W_3,
                            _, _,
                            2);
    

    // layer 11
    input = output;
    gap(input, output,
            FIL_3, H_3, W_3,
            FIL_3, 1, 1);
    

    // layer 12
    input = output;
    weight = load_data("parameters/fc_weight.bin");
    bias = load_data("parameters/fc_bias.bin");
    full_connected(input, output, weight, bias,
                    FIL_3, 1, 1,
                    CLASS, 
                    FIL_3, CLASS,
                    CLASS);
    
    free(weight);
    free(bias);
    input = nullptr;
    weight = nullptr;
    bias = nullptr;

    data_gather(output, out, N*CLASS);

    free(output);
    free(residual_input);
}
