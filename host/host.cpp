#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <typeinfo>

#include <xrt.h>
#include <experimental/xrt_kernel.h>

#define AIE_KERNEL_NUMBER 15

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    // Get device index and download xclbin
    std::cout << "Open the device" << std::endl;
    auto device = xrt::device(0);
    std::string binaryFile = argv[1];
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    // Get reference to the kernels
    std::cout << "Get references to compute units" << std::endl;
    auto tile_mm2mm_1 = xrt::kernel(device, uuid, "tile_mm2mm:{tile_mm2mm1}");
    auto sitcker_mm2mm_1 = xrt::kernel(device, uuid, "sticker_mm2mm:{sticker_mm2mm1}");
    std::array<xrt::kernel, AIE_KERNEL_NUMBER> mm2s_;
    std::array<xrt::kernel, AIE_KERNEL_NUMBER> s2mm_;
    for (unsigned i = 0; i < mm2s_.size(); ++i) {
        std::string cu_name = "mm2s:{mm2s" + std::to_string(i+1) + "}";
        mm2s_[i] = xrt::kernel(device, uuid, cu_name.c_str());
    }
    for (unsigned i = 0; i < s2mm_.size(); ++i) {
        std::string cu_name = "s2mm:{s2mm" + std::to_string(i+1) + "}";
        s2mm_[i] = xrt::kernel(device, uuid, cu_name.c_str());
    }

    // Allocating the input size of sizeIn to MM2S
    std::cout << "Allocate Buffer in Global Memory" << std::endl;
    unsigned img_width     = 720;
    unsigned img_height = 480;
    unsigned img_number = 2;
    unsigned img_num_elements = img_width * img_height * img_number;

    unsigned tile_width = 64;
    unsigned tile_height = 32;
    unsigned data_width = 32;

    unsigned tile_num_width = ceil((float)(img_width - tile_width) / (tile_width - 2)) + 1;
	unsigned tile_num_height = ceil((float)(img_height - tile_height) / (tile_height - 2)) + 1;

    unsigned iteration = ceil((float)(tile_num_width * tile_num_height) / AIE_KERNEL_NUMBER) * img_number;
    
    unsigned tile_num_elements = tile_width * tile_height;
    size_t img_size_in_bytes = sizeof(int) * img_num_elements;
    size_t tile_size_in_bytes = sizeof(int) * tile_num_elements * iteration;
    auto img_in_buff = xrt::bo(device, img_size_in_bytes, tile_mm2mm_1.group_id(0));

    std::array<xrt::bo, AIE_KERNEL_NUMBER> tiled_in_buff_;
    for (unsigned i = 1; i <= tiled_in_buff_.size(); ++i) {
        tiled_in_buff_[i] = xrt::bo(device, tile_size_in_bytes, tile_mm2mm_1.group_id(i));
    }
    
    std::array<xrt::bo, AIE_KERNEL_NUMBER> in_buff_;
    for (unsigned i = 0; i < in_buff_.size(); ++i) {
        in_buff_[i] = xrt::bo(device, tile_size_in_bytes, mm2s_[i].group_id(0));
    }

    auto img_out_buff = xrt::bo(device, img_size_in_bytes, sticker_mm2mm_1.group_id(15));

    std::array<xrt::bo, AIE_KERNEL_NUMBER> tiled_out_buff_;
    for (unsigned i = 0; i < tiled_out_buff_.size(); ++i) {
        tiled_out_buff_[i] = xrt:bo(device, tile_size_in_bytes, sticker_mm2mm_1.group_id(i));
    }

    std::array<xrt::bo, AIE_KERNEL_NUMBER> out_buff_;
    for (unsigned i = 0; i < out_buff_.size(); ++i) {
        out_buff_[i] = xrt::bo(device, tile_size_in_bytes, s2mm_[i].group_id(0));
    }

    // Read data from file 
    auto *DataInput = new int [img_num_elements];

    auto *DataOutput = new int [img_num_elements];

    for (unsigned int i = 0; i < img_num_elements; i++) {
        DataInput[i] = (rand() % (1 << 30)) * ((rand()%2) ? 1 : -1);
    }

    img_in_buff.write(DataInput);

    // Synchronize input buffers data to device global memory
    std::cout << "Synchronize input buffers data to device global memory" << std::endl;
    img_in_buff.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    // Execute the compute units
    std::cout << "Run the kernels" << std::endl;

    //TODO:tiler interface?
    auto run_tile_mm2mm_1 = tile_mm2mm_1(
	    img_in_buff, 
	    in_buff_[1], in_buff_[2], in_buff_[3], in_buff_[4], in_buff_[5],
	    in_buff_[6], in_buff_[7], in_buff_[8], in_buff_[9], in_buff_[10],
	    in_buff_[11], in_buff_[12], in_buff_[13], in_buff_[14], in_buff_[15]);
    run_tile_mm2mm_1.wait();

    for (unsigned i = 0; i < in_buff_.size(); ++i) {
        in_buff_[i].copy(tile_in_buff_[i], tile_size_in_bytes);
    }

    std::array<xrt::run, AIE_KERNEL_NUMBER> run_s2mm_;
    for (unsigned i = 0; i < AIE_KERNEL_NUMBER; ++i) {
        run_s2mm_[i] = s2mm_[i](out_buff_[i], nullptr, tile_size_in_bytes);
    }
 
    std::array<xrt::run, AIE_KERNEL_NUMBER> run_mm2s_;
    for (unsigned i = 0; i < AIE_KERNEL_NUMBER; ++i) {
        run_mm2s_[i] = mm2s_[i](in_buff_[i], nullptr, tile_size_in_bytes);
    }

    // Wait for kernels to complete
    for (unsigned i = 0; i < AIE_KERNEL_NUMBER; ++i) {
        run_mm2s_[i].wait();
        std::cout << "mm2s_" << std::to_string(i) << " completed" << std::endl;
    }

    for (unsigned i = 0; i < AIE_KERNEL_NUMBER; ++i) {
        run_s2mm_[i].wait();
        std::cout << "s2mm_" << std::to_string(i) << " completed" << std::endl;
    }

    for (unsigned i = 0; i < AIE_KERNEL_NUMBER; ++i) {
        tiled_out_buff_[i].copy(out_buff_[i], tile_size_in_bytes);
    }

    //TODO:sticker interface?
    auto run_sticker_mm2mm_1 = sticker_mm2mm_1(
	    out_buff_[1], out_buff_[2], out_buff_[3], out_buff_[4], out_buff_[5],
	    out_buff_[6], out_buff_[7], out_buff_[8], out_buff_[9], out_buff_[10],
	    out_buff_[11], out_buff_[12], out_buff_[13], out_buff_[14], out_buff_[15],
	    img_out_buff);
    run_sticker_mm2mm_1.wait();

    // Synchronize the output buffer data from the device
    img_out_buff.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    // Read output buffer data to local buffer
    img_out_buff.read(DataOutput);

    std::cout << "Writing data to output file" << std::endl;
    std::ofstream outputfile;
    outputfile.open("build.hw/aie_hw_run_data/output.txt");
    for (unsigned i = 0; i < img_num_elements; i++) {
        outputfile << DataOutput[i] << std::endl;
    }
    outputfile.close();

    return 0;
}
