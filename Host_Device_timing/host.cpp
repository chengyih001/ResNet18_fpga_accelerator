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

void gap(d_type* in_buffer, d_type* out_buffer,
            int in_fil, int in_h, int in_w,
            int out_fil, int out_h, int out_w)
{
    int n, c, h, w;
    for (n=0; n < N; n++) {
        for (c=0; c < out_fil; c++) {
            for (h=0; h < in_h; h++) {
                for (w=0; w < in_w; w++) {
                    out_buffer[n*out_fil + c] = out_buffer[n*out_fil + c] + in_buffer[n*out_fil*in_h*in_w + c*in_h*in_w + h*in_w + w];
                }
            }
            out_buffer[n*out_fil + c] /= (in_h * in_w);
        }
    }
}

void full_connected(d_type* in_buffer, d_type* out_buffer, d_type* weight_buffer, d_type* bias_buffer,
                        int in_fil, int out_size,
                        int wei_h, int wei_w, int bias_size)
{
    int n, m, d;

    // matrix multipllication: [N][64] x [64][10] -> [N][10]
    for (n=0; n < N; n++) {
        for (m=0; m < out_size; m++) {
            out_buffer[n*out_size+m] = bias_buffer[m];
            for (d=0; d < in_fil; d++) {
                out_buffer[n*out_size+m] += in_buffer[n*in_fil+d] * weight_buffer[m*in_fil+d];
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

    std::vector<d_type, aligned_allocator<d_type>> dummy(1);
    dummy[0] = 0.0;

    // std::vector<d_type, aligned_allocator<d_type>> y(get_file_size("parameters/validate_images.bin"));
    // std::vector<d_type, aligned_allocator<d_type>> fc_weight(get_file_size("parameters/fc_weight.bin"));
    // std::vector<d_type, aligned_allocator<d_type>> fc_bias(get_file_size("parameters/fc_bias.bin"));

    d_type* y = load_data("parameters/validate_images.bin");
    d_type* fc_weight = load_data("parameters/fc_weight.bin");
    d_type* fc_bias = load_data("parameters/fc_bias.bin");

    std::vector<d_type, aligned_allocator<d_type>> bn_weight_0(get_file_size("parameters/bn_weight_0.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_1_1(get_file_size("parameters/bn_weight_1_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_1_2(get_file_size("parameters/bn_weight_1_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_2_1(get_file_size("parameters/bn_weight_2_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_2_2(get_file_size("parameters/bn_weight_2_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_3_1(get_file_size("parameters/bn_weight_3_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_3_2(get_file_size("parameters/bn_weight_3_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_4_1(get_file_size("parameters/bn_weight_4_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_4_2(get_file_size("parameters/bn_weight_4_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_5_1(get_file_size("parameters/bn_weight_5_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_5_2(get_file_size("parameters/bn_weight_5_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_6_1(get_file_size("parameters/bn_weight_6_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_6_2(get_file_size("parameters/bn_weight_6_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_7_1(get_file_size("parameters/bn_weight_7_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_7_2(get_file_size("parameters/bn_weight_7_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_8_1(get_file_size("parameters/bn_weight_8_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_8_2(get_file_size("parameters/bn_weight_8_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_9_1(get_file_size("parameters/bn_weight_9_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> bn_weight_9_2(get_file_size("parameters/bn_weight_9_2.bin")/sizeof(d_type));

    std::vector<d_type, aligned_allocator<d_type>> conv_bias_0(get_file_size("parameters/conv_bias_0.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_1_1(get_file_size("parameters/conv_bias_1_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_1_2(get_file_size("parameters/conv_bias_1_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_2_1(get_file_size("parameters/conv_bias_2_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_2_2(get_file_size("parameters/conv_bias_2_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_3_1(get_file_size("parameters/conv_bias_3_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_3_2(get_file_size("parameters/conv_bias_3_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_4_1(get_file_size("parameters/conv_bias_4_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_4_2(get_file_size("parameters/conv_bias_4_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_5_1(get_file_size("parameters/conv_bias_5_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_5_2(get_file_size("parameters/conv_bias_5_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_6_1(get_file_size("parameters/conv_bias_6_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_6_2(get_file_size("parameters/conv_bias_6_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_7_1(get_file_size("parameters/conv_bias_7_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_7_2(get_file_size("parameters/conv_bias_7_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_8_1(get_file_size("parameters/conv_bias_8_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_8_2(get_file_size("parameters/conv_bias_8_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_9_1(get_file_size("parameters/conv_bias_9_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_bias_9_2(get_file_size("parameters/conv_bias_9_2.bin")/sizeof(d_type));

    std::vector<d_type, aligned_allocator<d_type>> conv_weight_0(get_file_size("parameters/conv_weight_0.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_1_1(get_file_size("parameters/conv_weight_1_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_1_2(get_file_size("parameters/conv_weight_1_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_2_1(get_file_size("parameters/conv_weight_2_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_2_2(get_file_size("parameters/conv_weight_2_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_3_1(get_file_size("parameters/conv_weight_3_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_3_2(get_file_size("parameters/conv_weight_3_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_4_1(get_file_size("parameters/conv_weight_4_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_4_2(get_file_size("parameters/conv_weight_4_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_5_1(get_file_size("parameters/conv_weight_5_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_5_2(get_file_size("parameters/conv_weight_5_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_6_1(get_file_size("parameters/conv_weight_6_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_6_2(get_file_size("parameters/conv_weight_6_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_7_1(get_file_size("parameters/conv_weight_7_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_7_2(get_file_size("parameters/conv_weight_7_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_8_1(get_file_size("parameters/conv_weight_8_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_8_2(get_file_size("parameters/conv_weight_8_2.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_9_1(get_file_size("parameters/conv_weight_9_1.bin")/sizeof(d_type));
    std::vector<d_type, aligned_allocator<d_type>> conv_weight_9_2(get_file_size("parameters/conv_weight_9_2.bin")/sizeof(d_type));

    load_data_to_vec(in, "parameters/testing_images.bin");

    load_data_to_vec(bn_weight_0, "parameters/bn_weight_0.bin");
    load_data_to_vec(bn_weight_1_1, "parameters/bn_weight_1_1.bin");
    load_data_to_vec(bn_weight_1_2, "parameters/bn_weight_1_2.bin");
    load_data_to_vec(bn_weight_2_1, "parameters/bn_weight_2_1.bin");
    load_data_to_vec(bn_weight_2_2, "parameters/bn_weight_2_2.bin");
    load_data_to_vec(bn_weight_3_1, "parameters/bn_weight_3_1.bin");
    load_data_to_vec(bn_weight_3_2, "parameters/bn_weight_3_2.bin");
    load_data_to_vec(bn_weight_4_1, "parameters/bn_weight_4_1.bin");
    load_data_to_vec(bn_weight_4_2, "parameters/bn_weight_4_2.bin");
    load_data_to_vec(bn_weight_5_1, "parameters/bn_weight_5_1.bin");
    load_data_to_vec(bn_weight_5_2, "parameters/bn_weight_5_2.bin");
    load_data_to_vec(bn_weight_6_1, "parameters/bn_weight_6_1.bin");
    load_data_to_vec(bn_weight_6_2, "parameters/bn_weight_6_2.bin");
    load_data_to_vec(bn_weight_7_1, "parameters/bn_weight_7_1.bin");
    load_data_to_vec(bn_weight_7_2, "parameters/bn_weight_7_2.bin");
    load_data_to_vec(bn_weight_8_1, "parameters/bn_weight_8_1.bin");
    load_data_to_vec(bn_weight_8_2, "parameters/bn_weight_8_2.bin");
    load_data_to_vec(bn_weight_9_1, "parameters/bn_weight_9_1.bin");
    load_data_to_vec(bn_weight_9_2, "parameters/bn_weight_9_2.bin");

    load_data_to_vec(conv_bias_0, "parameters/conv_bias_0.bin");
    load_data_to_vec(conv_bias_1_1, "parameters/conv_bias_1_1.bin");
    load_data_to_vec(conv_bias_1_2, "parameters/conv_bias_1_2.bin");
    load_data_to_vec(conv_bias_2_1, "parameters/conv_bias_2_1.bin");
    load_data_to_vec(conv_bias_2_2, "parameters/conv_bias_2_2.bin");
    load_data_to_vec(conv_bias_3_1, "parameters/conv_bias_3_1.bin");
    load_data_to_vec(conv_bias_3_2, "parameters/conv_bias_3_2.bin");
    load_data_to_vec(conv_bias_4_1, "parameters/conv_bias_4_1.bin");
    load_data_to_vec(conv_bias_4_2, "parameters/conv_bias_4_2.bin");
    load_data_to_vec(conv_bias_5_1, "parameters/conv_bias_5_1.bin");
    load_data_to_vec(conv_bias_5_2, "parameters/conv_bias_5_2.bin");
    load_data_to_vec(conv_bias_6_1, "parameters/conv_bias_6_1.bin");
    load_data_to_vec(conv_bias_6_2, "parameters/conv_bias_6_2.bin");
    load_data_to_vec(conv_bias_7_1, "parameters/conv_bias_7_1.bin");
    load_data_to_vec(conv_bias_7_2, "parameters/conv_bias_7_2.bin");
    load_data_to_vec(conv_bias_8_1, "parameters/conv_bias_8_1.bin");
    load_data_to_vec(conv_bias_8_2, "parameters/conv_bias_8_2.bin");
    load_data_to_vec(conv_bias_9_1, "parameters/conv_bias_9_1.bin");
    load_data_to_vec(conv_bias_9_2, "parameters/conv_bias_9_2.bin");

    load_data_to_vec(conv_weight_0, "parameters/conv_weight_0.bin");
    load_data_to_vec(conv_weight_1_1, "parameters/conv_weight_1_1.bin");
    load_data_to_vec(conv_weight_1_2, "parameters/conv_weight_1_2.bin");
    load_data_to_vec(conv_weight_2_1, "parameters/conv_weight_2_1.bin");
    load_data_to_vec(conv_weight_2_2, "parameters/conv_weight_2_2.bin");
    load_data_to_vec(conv_weight_3_1, "parameters/conv_weight_3_1.bin");
    load_data_to_vec(conv_weight_3_2, "parameters/conv_weight_3_2.bin");
    load_data_to_vec(conv_weight_4_1, "parameters/conv_weight_4_1.bin");
    load_data_to_vec(conv_weight_4_2, "parameters/conv_weight_4_2.bin");
    load_data_to_vec(conv_weight_5_1, "parameters/conv_weight_5_1.bin");
    load_data_to_vec(conv_weight_5_2, "parameters/conv_weight_5_2.bin");
    load_data_to_vec(conv_weight_6_1, "parameters/conv_weight_6_1.bin");
    load_data_to_vec(conv_weight_6_2, "parameters/conv_weight_6_2.bin");
    load_data_to_vec(conv_weight_7_1, "parameters/conv_weight_7_1.bin");
    load_data_to_vec(conv_weight_7_2, "parameters/conv_weight_7_2.bin");
    load_data_to_vec(conv_weight_8_1, "parameters/conv_weight_8_1.bin");
    load_data_to_vec(conv_weight_8_2, "parameters/conv_weight_8_2.bin");
    load_data_to_vec(conv_weight_9_1, "parameters/conv_weight_9_1.bin");
    load_data_to_vec(conv_weight_9_2, "parameters/conv_weight_9_2.bin");



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

    // Allocate device memory
    OCL_CHECK(err, cl::Buffer buffer_in(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/testing_images.bin"), in.data(), &err));

    OCL_CHECK(err, cl::Buffer buffer_dummy(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(d_type), dummy.data(), &err));
    
    OCL_CHECK(err, cl::Buffer buffer_out(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, N*FIL_1*H_1*W_1*sizeof(d_type), out.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_residual(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, N*FIL_1*H_1*W_1*sizeof(d_type), residual.data(), &err));

    OCL_CHECK(err, cl::Buffer buffer_bn_weight_0(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_0.bin"), bn_weight_0.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_1_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_1_1.bin"), bn_weight_1_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_1_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_1_2.bin"), bn_weight_1_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_2_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_2_1.bin"), bn_weight_2_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_2_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_2_2.bin"), bn_weight_2_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_3_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_3_1.bin"), bn_weight_3_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_3_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_3_2.bin"), bn_weight_3_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_4_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_4_1.bin"), bn_weight_4_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_4_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_4_2.bin"), bn_weight_4_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_5_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_5_1.bin"), bn_weight_5_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_5_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_5_2.bin"), bn_weight_5_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_6_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_6_1.bin"), bn_weight_6_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_6_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_6_2.bin"), bn_weight_6_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_7_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_7_1.bin"), bn_weight_7_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_7_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_7_2.bin"), bn_weight_7_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_8_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_8_1.bin"), bn_weight_8_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_8_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_8_2.bin"), bn_weight_8_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_9_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_9_1.bin"), bn_weight_9_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bn_weight_9_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/bn_weight_9_2.bin"), bn_weight_9_2.data(), &err));

    OCL_CHECK(err, cl::Buffer buffer_conv_bias_0(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_0.bin"), conv_bias_0.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_1_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_1_1.bin"), conv_bias_1_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_1_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_1_2.bin"), conv_bias_1_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_2_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_2_1.bin"), conv_bias_2_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_2_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_2_2.bin"), conv_bias_2_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_3_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_3_1.bin"), conv_bias_3_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_3_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_3_2.bin"), conv_bias_3_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_4_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_4_1.bin"), conv_bias_4_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_4_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_4_2.bin"), conv_bias_4_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_5_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_5_1.bin"), conv_bias_5_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_5_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_5_2.bin"), conv_bias_5_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_6_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_6_1.bin"), conv_bias_6_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_6_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_6_2.bin"), conv_bias_6_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_7_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_7_1.bin"), conv_bias_7_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_7_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_7_2.bin"), conv_bias_7_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_8_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_8_1.bin"), conv_bias_8_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_8_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_8_2.bin"), conv_bias_8_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_9_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_9_1.bin"), conv_bias_9_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_bias_9_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_bias_9_2.bin"), conv_bias_9_2.data(), &err));

    OCL_CHECK(err, cl::Buffer buffer_conv_weight_0(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_0.bin"), conv_weight_0.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_1_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_1_1.bin"), conv_weight_1_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_1_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_1_2.bin"), conv_weight_1_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_2_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_2_1.bin"), conv_weight_2_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_2_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_2_2.bin"), conv_weight_2_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_3_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_3_1.bin"), conv_weight_3_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_3_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_3_2.bin"), conv_weight_3_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_4_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_4_1.bin"), conv_weight_4_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_4_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_4_2.bin"), conv_weight_4_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_5_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_5_1.bin"), conv_weight_5_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_5_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_5_2.bin"), conv_weight_5_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_6_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_6_1.bin"), conv_weight_6_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_6_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_6_2.bin"), conv_weight_6_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_7_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_7_1.bin"), conv_weight_7_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_7_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_7_2.bin"), conv_weight_7_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_8_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_8_1.bin"), conv_weight_8_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_8_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_8_2.bin"), conv_weight_8_2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_9_1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_9_1.bin"), conv_weight_9_1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_conv_weight_9_2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, get_file_size("parameters/conv_weight_9_2.bin"), conv_weight_9_2.data(), &err));
    
    // Copy input data to device global memory
    OCL_CHECK(err, err = queue.enqueueMigrateMemObjects({buffer_in, buffer_bn_weight_0, buffer_bn_weight_1_1, buffer_bn_weight_1_2, buffer_bn_weight_2_1, buffer_bn_weight_2_2, buffer_bn_weight_3_1, buffer_bn_weight_3_2, 
                                                                                        buffer_bn_weight_4_1, buffer_bn_weight_4_2, buffer_bn_weight_5_1, buffer_bn_weight_5_2, buffer_bn_weight_6_1, buffer_bn_weight_6_2,
                                                                                        buffer_bn_weight_7_1, buffer_bn_weight_7_2, buffer_bn_weight_8_1, buffer_bn_weight_8_2, buffer_bn_weight_9_1, buffer_bn_weight_9_2,
                                                                    buffer_conv_bias_0, buffer_conv_bias_1_1, buffer_conv_bias_1_2, buffer_conv_bias_2_1, buffer_conv_bias_2_2, buffer_conv_bias_3_1, buffer_conv_bias_3_2,
                                                                                        buffer_conv_bias_4_1, buffer_conv_bias_4_2, buffer_conv_bias_5_1, buffer_conv_bias_5_2, buffer_conv_bias_6_1, buffer_conv_bias_6_2,
                                                                                        buffer_conv_bias_7_1, buffer_conv_bias_7_2, buffer_conv_bias_8_1, buffer_conv_bias_8_2, buffer_conv_bias_9_1, buffer_conv_bias_9_2,
                                                                    buffer_conv_weight_0, buffer_conv_weight_1_1, buffer_conv_weight_1_2, buffer_conv_weight_2_1, buffer_conv_weight_2_2, buffer_conv_weight_3_1, buffer_conv_weight_3_2,
                                                                                          buffer_conv_weight_4_1, buffer_conv_weight_4_2, buffer_conv_weight_5_1, buffer_conv_weight_5_2, buffer_conv_weight_6_1, buffer_conv_weight_6_2,
                                                                                          buffer_conv_weight_7_1, buffer_conv_weight_7_2, buffer_conv_weight_8_1, buffer_conv_weight_8_2, buffer_conv_weight_9_1, buffer_conv_weight_9_2,
                                                                                        buffer_dummy}, 0));

    OCL_CHECK(err, err = queue.finish());


    auto start = std::chrono::high_resolution_clock::now();

// layer 0
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_in));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_0));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_0));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, 3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, 3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, 32));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, 32));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_0));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

// layer 1
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

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_1_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 1));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_1_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_1_2));
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

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_1_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err =  krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
            
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 3));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

// layer 2
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_2_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_2_1));
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

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_2_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
    
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 1));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_2_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_2_2));
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

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_2_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 3));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

// layer 3
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_3_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_3_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_3_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
    
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 1));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_3_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_3_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_3_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 3));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

// layer 4
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_4_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_4_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_4_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
   
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 1));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_4_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_4_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
 
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_4_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 3));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

// layer 5
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_5_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_5_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_5_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
    
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 1));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_5_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_5_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_5_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
    
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 3));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

// layer 6
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_6_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_6_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_6_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
   
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 1));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_6_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_6_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_6_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 3));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

// layer 7
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_7_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_7_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_7_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
    
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 1));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_7_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_7_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_7_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
    
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 3));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

// layer 8
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_8_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_8_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_8_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
    
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 1));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_8_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_8_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_8_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
    
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 3));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

// layer 9
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_9_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_9_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_9_1));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
    
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 1));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_conv_weight_9_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_conv_bias_9_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, CONV_H));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, CONV_W));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 0));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));
    
    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_bn_weight_9_2));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 1));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));
    
    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_residual));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 3));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));

    OCL_CHECK(err, err = krnl_resnet.setArg(0, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(1, buffer_out));
    OCL_CHECK(err, err = krnl_resnet.setArg(2, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(3, buffer_dummy));
    OCL_CHECK(err, err = krnl_resnet.setArg(4, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(5, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(6, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(7, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(8, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(9, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(10, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(11, FIL_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(12, H_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(13, W_3));
    OCL_CHECK(err, err = krnl_resnet.setArg(14, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(15, _));
    OCL_CHECK(err, err = krnl_resnet.setArg(16, 2));
    OCL_CHECK(err, err = krnl_resnet.setArg(17, 0));

    // Launch Kernel
    OCL_CHECK(err, err = queue.enqueueTask(krnl_resnet));


    OCL_CHECK(err, err = queue.finish());

    // Copy results from device global memory to host local memory
    OCL_CHECK(err, err = queue.enqueueMigrateMemObjects({buffer_out}, CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = queue.finish());

    d_type* in_buffer = (d_type *)calloc(N*FIL_3*H_3*W_3, sizeof(d_type));
    d_type* out_buffer = (d_type *)calloc(N*FIL_3, sizeof(d_type));
    d_type score[N*CLASS];


    for (int i=0; i < N*FIL_3*H_3*W_3; i++) {
        in_buffer[i] = out[i];
    }

    gap(in_buffer, out_buffer, FIL_3, H_3, W_3, FIL_3, 1, 1);


    full_connected(out_buffer, score, fc_weight, fc_bias, FIL_3, CLASS, FIL_3, CLASS, CLASS);



    auto end = std::chrono::high_resolution_clock::now();
    auto exec_time = std::chrono::duration<double>(end - start).count();

    // Compare results
    int y_pred[N];
    float max_score = 0.0;
    for (int i=0; i < N; i++) {
        max_score = score[i*CLASS];
        y_pred[i] = 0;
        for (int j=1; j < CLASS; j++) {
            if (score[i*CLASS + j] > max_score) {
                max_score = score[i*CLASS + j];
                y_pred[i] = j;
            }
        }
    }

    float sum = 0.0;
    float total = 0.0;
    for (int i=0; i < N; i++) {
        total += 1.0;
        if (y_pred[i] == int(y[i])) {
            sum += 1.0;
        }
    }

    printf("accuracy = %.2f%%\n", float(100*sum/total));
    printf("FPGA kernel exec time is %f s\n", exec_time);

    free(in_buffer);
    free(out_buffer);
    free(y);
    free(fc_weight);
    free(fc_bias);

}