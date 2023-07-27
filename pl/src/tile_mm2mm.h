#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>
#include <math.h>

// 对一张图片的指定位置进行 tile 操作
template <unsigned img_width, unsigned img_height, unsigned img_number, unsigned tile_width, unsigned tile_height, unsigned aie_kernel_num, unsigned data_width>
void tile_mm2mm(ap_int<data_width> *mem_in, 
ap_int<data_width> *mem_out1, ap_int<data_width> *mem_out2, ap_int<data_width> *mem_out3, ap_int<data_width> *mem_out4, 
ap_int<data_width> *mem_out5, ap_int<data_width> *mem_out6, ap_int<data_width> *mem_out7, ap_int<data_width> *mem_out8, ap_int<data_width> *mem_out9, ap_int<data_width> *mem_out10,
ap_int<data_width> *mem_out11, ap_int<data_width> *mem_out12, ap_int<data_width> *mem_out13, ap_int<data_width> *mem_out14, ap_int<data_width> *mem_out15, 
unsgined tile_index_width, unsigned tile_index_height) {


    // 计算一张图片有多少 tile
    unsigned tile_num_width  = ceil((float)(img_width - tile_width) / (tile_width - 2)) + 1;
    unsigned tile_num_height = ceil((float)(img_height - tile_height) / (tile_height - 2)) + 1;

    unsigned count[15] = {0};

    // 遍历所有图片
    for (unsigned i = 0; i < img_number; i++) {

        // 遍历所有的 tile
        for (unsigned j = 0; j < tile_num_height; j++) {
            for (unsigned k = 0; k < tile_num_width; k++) {

                // 当前的 tile 应该传输给第 aie_index 个 aie kernel
                unsigned aie_index = (j * tile_num_width + k) % aie_kernel_num + 1;
                
                // 当前 tile 相对于第一个元素的偏移
                unsigned offset_width  = k * (tile_width - 2);
                unsigned offset_height = j * (tile_height - 2);

                // 遍历当前的 tile
                for (unsigned ti = 0; ti < tile_height; ti++) {
                    for (unsigned tj = 0; tj < tile_width; tj++) {

                        int index = 0;
                        
                        // 判断是否遍历到图片边缘
                        bool edge_flag_width  = ((tj + offset_width) == (img_width));
                        bool edge_flag_height = ((ti + offset_height) == (img_height));

                        // 遍历到图片边缘后需要进行 padding 操作 
                        // index == -1 表示补零
                        if (!edge_flag_height && !edge_flag_width)
                            index = (ti + offset_height) * img_width + tj + offset_width;                        
                        else if (edge_flag_height && !edge_flag_width)
                            index = (img_height - 1) * img_width + tj + offset_width;
                        else if (!edge_flag_height && edge_flag_width)
                            index = (ti + offset_height) * img_width + img_width - 1;
                        else if (edge_flag_height && edge_flag_width)
                            index = (img_height - 1) * img_width + img_width - 1;
                        else
                            index = -1;

                        // 将分块好的数据存入对应 aie 所读取的 mem 区域
                        switch(aie_index) {
                            case 1:
                                if (index = -1)
                                    mem_out1[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out1[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 2:
                                if (index = -1)
                                    mem_out2[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out2[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 3:
                                if (index = -1)
                                    mem_out3[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out3[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 4:
                                if (index = -1)
                                    mem_out4[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out4[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 5:
                                if (index = -1)
                                    mem_out5[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out5[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 6:
                                if (index = -1)
                                    mem_out6[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out6[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 7:
                                if (index = -1)
                                    mem_out7[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out7[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 8:
                                if (index = -1)
                                    mem_out8[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out8[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 9:
                                if (index = -1)
                                    mem_out9[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out9[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 10:
                                if (index = -1)
                                    mem_out10[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out10[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 11:
                                if (index = -1)
                                    mem_out11[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out11[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 12:
                                if (index = -1)
                                    mem_out12[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out12[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 13:
                                if (index = -1)
                                    mem_out13[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out13[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 14:
                                if (index = -1)
                                    mem_out14[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out14[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            case 15:
                                if (index = -1)
                                    mem_out15[aie_index][count[aie_index]++] = 0;
                                else 
                                    mem_out15[aie_index][count[aie_index]++] = mem_in[index];
                                break;
                            default:
                                printf("aie index = %d\n", aie_index);
                        }
                    }
                }
            }
        }
    }
}

// template<unsigned data_width>
// void mem_transfer(ap_int<data_width> *mem_in, 
// ap_int<data_width> *mem_out1, ap_int<data_width> *mem_out2, ap_int<data_width> *mem_out3, ap_int<data_width> *mem_out4, 
// ap_int<data_width> *mem_out5, ap_int<data_width> *mem_out6, ap_int<data_width> *mem_out7, ap_int<data_width> *mem_out8, ap_int<data_width> *mem_out9, ap_int<data_width> *mem_out10,
// ap_int<data_width> *mem_out11, ap_int<data_width> *mem_out12, ap_int<data_width> *mem_out13, ap_int<data_width> *mem_out14, ap_int<data_width> *mem_out15,
// unsigned aie_index, unsigned count[15], unsigned index) {

//     switch(aie_index) {
//         case 1:
//             if (index = -1)
//                 mem_out1[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out1[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 2:
//             if (index = -1)
//                 mem_out2[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out2[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 3:
//             if (index = -1)
//                 mem_out3[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out3[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 4:
//             if (index = -1)
//                 mem_out4[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out4[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 5:
//             if (index = -1)
//                 mem_out5[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out5[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 6:
//             if (index = -1)
//                 mem_out6[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out6[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 7:
//             if (index = -1)
//                 mem_out7[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out7[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 8:
//             if (index = -1)
//                 mem_out8[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out8[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 9:
//             if (index = -1)
//                 mem_out9[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out9[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 10:
//             if (index = -1)
//                 mem_out10[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out10[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 11:
//             if (index = -1)
//                 mem_out11[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out11[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 12:
//             if (index = -1)
//                 mem_out12[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out12[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 13:
//             if (index = -1)
//                 mem_out13[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out13[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 14:
//             if (index = -1)
//                 mem_out14[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out14[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         case 15:
//             if (index = -1)
//                 mem_out15[aie_index][count[aie_index]++] = 0;
//             else 
//                 mem_out15[aie_index][count[aie_index]++] = mem_in[index];
//             break;
//         default:
//             printf("aie index = %d\n", aie_index);
//     }

// }
//     std::map<int, char> mem_out_map;
//     struct
//     *mem_out[15]
