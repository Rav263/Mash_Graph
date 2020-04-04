#include <vector>
#include <iostream>

#include <glm/glm.hpp>

uint32_t get_num(uint32_t x, uint32_t y, uint32_t width) {
    return x + y * width;
}


glm::vec3 get_gray(glm::vec3 &color) {
    float gray = color[0] * 0.2126 + color[1] * 0.7152 + color[2] * 0.0722;
    return glm::vec3(gray, gray, gray);
}


void change_color_to_gray(std::vector<glm::vec3> &image, std::vector<glm::vec3> &gray_image, uint32_t width, uint32_t height, uint32_t threads_num) {
    #pragma omp parallel for num_threads(threads_num)
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            auto index = get_num(x, y, width);
            gray_image[index] = get_gray(image[index]);
        }
    }
}


float mattrix_x[3][3] =
    {{-1, 0, 1},
     {-2, 0, 2},
     {-1, 0, 1}};

float mattrix_y[3][3] =
    {{-1, -2, -1},
     { 0,  0,  0},
     { 1,  2,  1}};



void detect_image_edges(std::vector<glm::vec3> &image, std::vector<glm::vec3> &edges, uint32_t width, uint32_t height, uint32_t threads_num) {
    std::vector<glm::vec3> gray_image(image.size());

    change_color_to_gray(image, gray_image, width, height, threads_num);

    #pragma omp parallel for num_threads(threads_num)
    for (int32_t x = 1; x < width - 1; x++) {
        for (int32_t y = 1; y < height - 1; y++) {
            float gx = 0;
            for(int i = -1; i < 2; ++i) {
                for(int j = -1; j < 2; ++j) {
                    gx += mattrix_x[i + 1][j + 1] * gray_image[get_num(x + i, y + j, width)][0];
                }
            }
            
            float gy = 0;
            for(int32_t i = -1; i < 2; ++i) {
                for(int32_t j = -1; j < 2; ++j) {
                    gy += mattrix_y[i + 1][j + 1] * gray_image[get_num(x + i, y + j, width)][0];
                }
            }
            
            float grad = std::sqrt(gx * gx + gy * gy);
            edges[get_num(x, y, width)] = glm::vec3(grad, grad, grad);
        }
    }
}
