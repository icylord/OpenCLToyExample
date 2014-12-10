//
// Book:      OpenCL(R) Programming Guide
// Authors:   Aaftab Munshi, Benedict Gaster, Timothy Mattson, James Fung, Dan Ginsburg
// ISBN-10:   0-321-74964-2
// ISBN-13:   978-0-321-74964-2
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780132488006/
//            http://www.openclprogrammingguide.com
//


// vecadd.cpp
//
//    This is a simple example that demonstrates use OpenCL C++ Wrapper API.

// Enable OpenCL C++ exceptions
#define __CL_ENABLE_EXCEPTIONS


#include <CL/cl.hpp>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>

const int MATRIX_DIM = 512;
const int SUM_DIM = 64;
const int WORK_GROUP = 8;

int matrix[MATRIX_DIM][MATRIX_DIM];
int sum[SUM_DIM];

inline std::string loadProgram(std::string input)
{
    std::ifstream stream(input.c_str());
    if (!stream.is_open()) {
        printf("Cannot open input file\n");
        exit(1);
    }
    return std::string( std::istreambuf_iterator<char>(stream),
            (std::istreambuf_iterator<char>()));
}

int
main(void)
{
    cl_int err;

    // Initialize A, B, C
    int realsum = 0;
    for (int i = 0; i < MATRIX_DIM; ++i) {
        for (int j = 0; j < MATRIX_DIM; ++j) {
            matrix[i][j] = i;
            realsum += i;
        }
    }
    memset(sum, 0, sizeof(sum));

    try {
        std::vector<cl::Platform> platformList;

        // Pick platform
        cl::Platform::get(&platformList);

        // Pick first platform
        cl_context_properties cprops[] = {
            CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};
        cl::Context context(CL_DEVICE_TYPE_GPU, cprops);

        // Query the set of devices attched to the context
        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

        // Create and program from source
        std::string kernelSource = loadProgram("cl/summatrix.cl");
        cl::Program::Sources sources(1, std::make_pair(kernelSource.c_str(), 0));
        cl::Program program(context, sources);

        // Build program
        program.build(devices);

        // Create buffer for A and copy host contents
        cl::Buffer inBuffer = cl::Buffer(
            context, 
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
            MATRIX_DIM * MATRIX_DIM * sizeof(int),
            (void *) &matrix[0]);

        // Create buffer for that uses the host ptr C
        cl::Buffer outBuffer = cl::Buffer(
            context, 
            CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, 
            SUM_DIM * sizeof(int),
            (void *) &sum[0]);

        // Create kernel object
        cl::Kernel kernel(program, "sum_matrix");

        // Set kernel args
        kernel.setArg(0, inBuffer);
        kernel.setArg(1, outBuffer);

        // Create command queue
        cl::CommandQueue queue(context, devices[0], 0);
 
        // Do the work
        queue.enqueueNDRangeKernel(
            kernel, 
            cl::NullRange, 
            cl::NDRange(WORK_GROUP, WORK_GROUP),
                cl::NullRange);

        // Map cBuffer to host pointer. This enforces a sync with 
        // the host backing space, remember we choose GPU device.
        int * output = (int *) queue.enqueueMapBuffer(
            outBuffer,
            CL_TRUE, // block 
            CL_MAP_READ,
            0,
            SUM_DIM * sizeof(int));

        int sums = 0;
        for (int i = 0; i < SUM_DIM; i++) {
            std::cout << sum[i] << " ";
            sums += sum[i];
        }
        std::cout << std::endl;
        std::cout << realsum << std::endl;
        std::cout << sums << std::endl;

        // Finally release our hold on accessing the memory
        err = queue.enqueueUnmapMemObject(
            outBuffer,
            (void *) output);
 
        // There is no need to perform a finish on the final unmap
        // or release any objects as this all happens implicitly with
        // the C++ Wrapper API.
    } 
    catch (cl::Error err) {
         std::cerr
             << "ERROR: "
             << err.what()
             << "("
             << err.err()
             << ")"
             << std::endl;

         return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
