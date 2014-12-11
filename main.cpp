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

using namespace std;
// macros to reduce pollution
#define P(obj, w) cout << #w << ": " << obj.getInfo<w>() << "\n";
#define Pbool(obj, w) cout << std::boolalpha << #w << ": " << static_cast<bool>(obj.getInfo<w>()) << "\n";

#define PbitmapStart(obj, w) { unsigned bitmap = obj.getInfo<w>(); cout << #w << ": ";
#define PbitmapTest(w) if (bitmap & w) cout << #w " ";
#define PbitmapEnd cout << "\n"; }

#define PconstStart(obj, w) { unsigned constant = obj.getInfo<w>(); cout << #w << ": ";
#define PconstTest(w) if (constant == w) cout << #w "\n";
#define PconstEnd }

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

		// dump platform information
		for (auto platform : platformList) {
			P(platform, CL_PLATFORM_PROFILE);
			P(platform, CL_PLATFORM_VERSION);
			P(platform, CL_PLATFORM_NAME);
			P(platform, CL_PLATFORM_VENDOR);
			P(platform, CL_PLATFORM_EXTENSIONS);

			std::vector<cl::Device> devices;
			(void)platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
			cout << "Number of devices: " << devices.size() << "\n";

			// dump device information
			for (auto device : devices) {
				PbitmapStart(device, CL_DEVICE_TYPE);
				PbitmapTest(CL_DEVICE_TYPE_CPU);
				PbitmapTest(CL_DEVICE_TYPE_GPU);
				PbitmapTest(CL_DEVICE_TYPE_ACCELERATOR);
				PbitmapTest(CL_DEVICE_TYPE_DEFAULT);
				//PbitmapTest(CL_DEVICE_TYPE_CUSTOM);
				PbitmapEnd;
				P(device, CL_DEVICE_VENDOR_ID);
				P(device, CL_DEVICE_MAX_COMPUTE_UNITS);
				P(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);

				std::vector<size_t> sizes = device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
				cout << "CL_DEVICE_MAX_WORK_ITEM_SIZES: ";
				for (auto size : sizes) { cout << size << " "; }
				cout << "\n";

				P(device, CL_DEVICE_MAX_WORK_GROUP_SIZE);
				P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR);
				P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT);
				P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT);
				P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG);
				P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT);
				P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE);
				P(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF);
				P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR);
				P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT);
				P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_INT);
				P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG);
				P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT);
				P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE);
				P(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF);
				P(device, CL_DEVICE_MAX_CLOCK_FREQUENCY);
				P(device, CL_DEVICE_ADDRESS_BITS);
				P(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE);
				Pbool(device, CL_DEVICE_IMAGE_SUPPORT);
				P(device, CL_DEVICE_MAX_READ_IMAGE_ARGS);
				P(device, CL_DEVICE_MAX_WRITE_IMAGE_ARGS);
				P(device, CL_DEVICE_IMAGE2D_MAX_WIDTH);
				P(device, CL_DEVICE_IMAGE2D_MAX_HEIGHT);
				P(device, CL_DEVICE_IMAGE3D_MAX_WIDTH);
				P(device, CL_DEVICE_IMAGE3D_MAX_HEIGHT);
				P(device, CL_DEVICE_IMAGE3D_MAX_DEPTH);
				// P(device, CL_DEVICE_IMAGE_MAX_BUFFER_SIZE);
				// P(device, CL_DEVICE_IMAGE_MAX_ARRAY_SIZE);
				P(device, CL_DEVICE_MAX_SAMPLERS);
				P(device, CL_DEVICE_MAX_PARAMETER_SIZE);
				P(device, CL_DEVICE_MEM_BASE_ADDR_ALIGN);
				PbitmapStart(device, CL_DEVICE_SINGLE_FP_CONFIG);
				PbitmapTest(CL_FP_DENORM);
				PbitmapTest(CL_FP_INF_NAN);
				PbitmapTest(CL_FP_ROUND_TO_NEAREST);
				PbitmapTest(CL_FP_ROUND_TO_ZERO);
				PbitmapTest(CL_FP_ROUND_TO_INF);
				PbitmapTest(CL_FP_FMA);
				//PbitmapTest(CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT);
				PbitmapTest(CL_FP_SOFT_FLOAT);
				PbitmapEnd;
				PbitmapStart(device, CL_DEVICE_DOUBLE_FP_CONFIG);
				PbitmapTest(CL_FP_DENORM);
				PbitmapTest(CL_FP_INF_NAN);
				PbitmapTest(CL_FP_ROUND_TO_NEAREST);
				PbitmapTest(CL_FP_ROUND_TO_ZERO);
				PbitmapTest(CL_FP_ROUND_TO_INF);
				PbitmapTest(CL_FP_FMA);
				//PbitmapTest(CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT);
				PbitmapTest(CL_FP_SOFT_FLOAT);
				PbitmapEnd;
				PconstStart(device, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE);
				PconstTest(CL_NONE);
				PconstTest(CL_READ_ONLY_CACHE);
				PconstTest(CL_READ_WRITE_CACHE);
				PconstEnd;
				P(device, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
				P(device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
				P(device, CL_DEVICE_GLOBAL_MEM_SIZE);
				P(device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);
				P(device, CL_DEVICE_MAX_CONSTANT_ARGS);
				PconstStart(device, CL_DEVICE_LOCAL_MEM_TYPE);
				PconstTest(CL_NONE);
				PconstTest(CL_LOCAL);
				PconstTest(CL_GLOBAL);
				PconstEnd;
				P(device, CL_DEVICE_LOCAL_MEM_SIZE);
				Pbool(device, CL_DEVICE_ERROR_CORRECTION_SUPPORT);
				Pbool(device, CL_DEVICE_HOST_UNIFIED_MEMORY);
				P(device, CL_DEVICE_PROFILING_TIMER_RESOLUTION);
				Pbool(device, CL_DEVICE_ENDIAN_LITTLE);
				Pbool(device, CL_DEVICE_AVAILABLE);
				Pbool(device, CL_DEVICE_COMPILER_AVAILABLE);
				// P(device, CL_DEVICE_LINKER_AVAILABLE);
				PbitmapStart(device, CL_DEVICE_EXECUTION_CAPABILITIES);
				PbitmapTest(CL_EXEC_KERNEL);
				PbitmapTest(CL_EXEC_NATIVE_KERNEL);
				PbitmapEnd;
				PbitmapStart(device, CL_DEVICE_QUEUE_PROPERTIES);
				PbitmapTest(CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
				PbitmapTest(CL_QUEUE_PROFILING_ENABLE);
				PbitmapEnd;
				// P(device, CL_DEVICE_BUILT_IN_KERNELS);
				P(device, CL_DEVICE_PLATFORM);
				P(device, CL_DEVICE_NAME);
				P(device, CL_DEVICE_VENDOR);
				P(device, CL_DRIVER_VERSION);
				P(device, CL_DEVICE_PROFILE);
				P(device, CL_DEVICE_VERSION);
				//P(device, CL_DEVICE_OPENCL_C_VERSION);
				P(device, CL_DEVICE_EXTENSIONS);
				// P(device, CL_DEVICE_PRINTF_BUFFER_SIZE);
				// Pbool(device, CL_DEVICE_PREFERRED_INTEROP_USER_SYNC);
				// P(device, CL_DEVICE_PARENT_DEVICE);
				// P(device, CL_DEVICE_PARTITION_MAX_SUB_DEVICES);
				// P(device, CL_DEVICE_PARTITION_PROPERTIES);
				// P(device, CL_DEVICE_PARTITION_AFFINITY_DOMAIN);
				// P(device, CL_DEVICE_PARTITION_TYPE);
				// P(device, CL_DEVICE_REFERENCE_COUNT);
			}
		}

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
