#include "xcl2.hpp"
#include <sys/stat.h>
#include <stdio.h>

#define v_N 16

typedef float d_type;


#define N 5
#define FIL_1 16
#define FIL_2 32
#define FIL_3 64
#define H_1 32
#define W_1 32
#define H_2 16
#define W_2 16
#define H_3 8
#define W_3 8

#define CONV_H 3
#define CONV_W 3

#define _ -1

#define CLASS 10

#define C_COUNT 20

size_t get_file_size(const char* filename)
{
    struct stat buf;
    if(stat(filename, &buf)<0)
    {
        return 0;
    }
    return (size_t)buf.st_size;
}


void load_data_to_vec(std::vector<d_type, aligned_allocator<d_type>> &vec, const char* filename) {
    int data_num = get_file_size(filename)/sizeof(d_type);
	d_type* data_buf = (d_type *)calloc(data_num, sizeof(d_type));
	FILE *fp_w = fopen(filename, "rb");
	fread(data_buf, sizeof(d_type), data_num, fp_w);
	fclose(fp_w);
    for (int i=0; i < data_num; i++) {
        vec[i] = data_buf[i];
    }
    free(data_buf);
}

d_type* load_data(const char* filename)
{
    int data_num = get_file_size(filename)/4;
	d_type* data_buf = (d_type *)calloc(data_num, sizeof(d_type));
	FILE *fp_w = fopen(filename, "rb");
	fread(data_buf, sizeof(d_type), data_num, fp_w);
	fclose(fp_w);
    return data_buf;
}

// convolution layer
void conv(d_type* in_buffer, d_type* out_buffer, d_type* weight_buffer, d_type* bias_buffer,
            int wei_fil_0, int wei_fil_1, int wei_h, int wei_w,
            int in_fil, int in_h, int in_w, int out_fil, int out_h, int out_w,
            int stride=1, int pad=1)
{
    const int MAX_IN_PAD = N*FIL_1*(H_1+2)*(W_1+2);
    int n, c, h, w, f, hh, ww;


    // pad input
    d_type in_pad[MAX_IN_PAD];

    for (n=0; n < MAX_IN_PAD; n++) {
        in_pad[n] = 0.0;
    }

    for (n=0; n < N; n++) {
        for (c=0; c < in_fil; c++) {
            for (h=0; h < in_h; h++) {
                for (w=0; w < in_w; w++) {
                    in_pad[n*in_fil*(in_h+2*pad)*(in_w+2*pad) + c*(in_h+2*pad)*(in_w+2*pad) + (h+pad)*(in_w+2*pad) + (w+pad)] = in_buffer[n*in_fil*(in_h)*(in_w) + c*(in_h)*(in_w) + (h)*(in_w) + (w)];
                }
            }
        }
    }

	for (n=0; n < N; n++) {
		for (f=0; f < out_fil; f++) {
			for (h=0; h < out_h; h++) {
				for (w=0; w < out_w; w++) {
					out_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] = bias_buffer[f];
				}
			}
		}
	}

	d_type temp[9];
    for (n=0; n < N; n++) {
        for (f=0; f < out_fil; f++) {
            for (h=0; h < out_h; h++) {
                for (w=0; w < out_w; w++) {
                    for (c=0; c < in_fil; c++) {
                        for (hh=0; hh < wei_h; hh++) {
                            for (ww=0; ww < wei_w; ww++) {
                            	 temp[hh * wei_w + ww] = weight_buffer[f*in_fil*wei_h*wei_w + c*wei_h*wei_w + hh*wei_w + ww] * in_pad[n*in_fil*(in_h+2*pad)*(in_w+2*pad) + c*(in_h+2*pad)*(in_w+2*pad) + (h*stride+hh)*(in_w+2*pad) + (w*stride+ww)];
                            }
                        }
                        d_type sum1 = temp[0] + temp[1];
                        d_type sum2 = temp[2] + temp[3];
                        d_type sum3 = temp[4] + temp[5];
                        d_type sum4 = temp[6] + temp[7];
                        d_type sum5 = sum1 + sum2;
                        d_type sum6 = sum3 + sum4;
                        d_type sum7 = sum5 + sum6;
                        out_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] = out_buffer[n*out_fil*out_h*out_w + f*out_h*out_w + h*out_w + w] + sum7 + temp[8];
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN FILE>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];
    cl_int err;
    cl::Context context;
    cl::Kernel krnl_resnet;
    cl::CommandQueue queue;

    // Allocate host memory and load inputs
    std::vector<d_type, aligned_allocator<d_type>> out(N*FIL_1*H_1*W_1);    
    std::vector<d_type, aligned_allocator<d_type>> residual(N*FIL_1*H_1*W_1);  
    std::vector<d_type, aligned_allocator<d_type>> in(get_file_size("parameters/testing_images.bin")/sizeof(d_type));

    for (int i=0; i < N*FIL_1*H_1*W_1; i++) {
        out[i] = 0;
    }
    for (int i=0; i < N*FIL_1*H_1*W_1; i++) {
        residual[i] = 0;
    }

    std::vector<d_type, aligned_allocator<d_type>> dummy(1);
    dummy[0] = 0.0;

    // std::vector<d_type, aligned_allocator<d_type>> y(get_file_size("parameters/validate_images.bin"));
    // std::vector<d_type, aligned_allocator<d_type>> fc_weight(get_file_size("parameters/fc_weight.bin"));
    // std::vector<d_type, aligned_allocator<d_type>> fc_bias(get_file_size("parameters/fc_bias.bin"));

    d_type* y = load_data("parameters/validate_images.bin");
    d_type* fc_weight = load_data("parameters/fc_weight.bin");
    d_type* fc_bias = load_data("parameters/fc_bias.bin");










    d_type* input_conv1 = load_data("parameters/testing_images.bin");
    d_type* output_conv1 = (d_type*) calloc(N*FIL_1*H_1*W_1, sizeof(d_type));
    d_type* weight_conv1 = load_data("parameters/conv_weight_0.bin");
    d_type* bias_conv1 = load_data("parameters/conv_bias_0.bin");
    conv(input_conv1, output_conv1, weight_conv1, bias_conv1, FIL_1, 3, CONV_H, CONV_W, 3, 32, 32, FIL_1, H_1, W_1, 1, 1);

    std::vector<d_type, aligned_allocator<d_type>> bout_conv1(N*FIL_1*H_1*W_1);

    for (int i=0; i < N*FIL_1*H_1*W_1; i++) {
        bout_conv1[i] = output_conv1[i];
    }


    std::vector<d_type, aligned_allocator<d_type>> conv_bias_1_1(get_file_size("parameters/conv_bias_1_1.bin")/sizeof(d_type));

    std::vector<d_type, aligned_allocator<d_type>> conv_weight_1_1(get_file_size("parameters/conv_weight_1_1.bin")/sizeof(d_type));

    load_data_to_vec(in, "parameters/testing_images.bin");

    load_data_to_vec(conv_bias_1_1, "parameters/conv_bias_1_1.bin");

    load_data_to_vec(conv_weight_1_1, "parameters/conv_weight_1_1.bin");


    // OpenCL Host Code
    auto xilinx_devices = xcl::get_xil_devices();
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i=0; i < xilinx_devices.size(); i++) {
        OCL_CHECK(err, context = cl::Context(xilinx_devices[i], nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, queue = cl::CommandQueue(context, xilinx_devices[i], CL_QUEUE_PROFILING_ENABLE, &err));
        std::cout << "Programming xilinx_device " << i << " :" << xilinx_devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {xilinx_devices[i]}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed while programming xilinx_device " << i << " with xclbin file." << std::endl;
        }
        else {
            std::cout << "Xilinx_device " << i << " programmed successfully." << std::endl;
            OCL_CHECK(err, krnl_resnet = cl::Kernel(program, "vadd", &err));
            valid_device = true;
            break;
        }
    }
    if (!valid_device) {
        std::cout << "No device available!" << std::endl;
        exit(EXIT_FAILURE);
    }








    OCL_CHECK(err, cl::Buffer buffer_output_conv1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, N*FIL_1*H_1*W_1*sizeof(d_type), bout_conv1.data(), &err));












    // Allocate device memory
    OCL_CHECK(err, cl::Buffer buffer_in(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/testing_images.bin"), in.data(), &err));

    OCL_CHECK(err, cl::Buffer buffer_dummy(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(d_type), dummy.data(), &err));
    
    OCL_CHECK(err, cl::Buffer buffer_out(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, N*FIL_1*H_1*W_1*sizeof(d_type), out.data(), &err));

    OCL_CHECK(err, cl::Buffer buffer_conv_bias_1_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_1_1.bin"), conv_bias_1_1.data(), &err));

    OCL_CHECK(err, cl::Buffer buffer_conv_weight_1_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_1_1.bin"), conv_weight_1_1.data(), &err));

    OCL_CHECK(err, err = queue.enqueueMigrateMemObjects({buffer_output_conv1, buffer_conv_bias_1_1, buffer_conv_weight_1_1,}, 0));

    OCL_CHECK(err, err = queue.finish());


    auto start = std::chrono::high_resolution_clock::now();

// layer 0
    // OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_in));
    // OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    // OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_0));
    // OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_0));
    // OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_1));
    // OCL_CHECK(err, err = krnl_resnet.setArg(5, 3));
    // OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    // OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    // OCL_CHECK(err, err = krnl_resnet.setArg(8, 3));
    // OCL_CHECK(err, err = krnl_resnet.setArg(9, 32));
    // OCL_CHECK(err, err = krnl_resnet.setArg(10, 32));
    // OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    // OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    // OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    // OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    // OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    // OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    // OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));





// layer 1
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_output_conv1));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_1_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_1_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    for (int i=0; i < C_COUNT; i++) {
        OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
        OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
        OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_1_1));
        OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_1_1));
        OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_1));
        OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_1));
        OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
        OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
        OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
        OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
        OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
        OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
        OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
        OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
        OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
        OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
        OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
        OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
        
        // Launch Kernel
        OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
    }


    OCL_CHECK(err, err = queue.finish());

    // Copy results from device global memory to host local memory
    OCL_CHECK(err, err = queue.enqueueMigrateMemObjects({buffer_out}, CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = queue.finish());




    d_type* in_buffer = (d_type *)calloc(N*FIL_3*H_3*W_3, sizeof(d_type));
    d_type* out_buffer = (d_type *)calloc(N*FIL_3, sizeof(d_type));
    d_type score[N*CLASS];

    for (int i=0; i < 10; i++) {
        printf("%f ", out[i]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto exec_time = std::chrono::duration<double>(end - start).count();

    printf("FPGA kernel exec time is %f s\n", exec_time);


    free(in_buffer);
    free(out_buffer);
    free(y);
    free(fc_weight);
    free(fc_bias);

    free(input_conv1);
    free(output_conv1);
    free(weight_conv1);
    free(bias_conv1);








}