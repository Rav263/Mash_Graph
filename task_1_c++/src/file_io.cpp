#include <glm/glm.hpp>
#include <vector>
#include <fstream>

void print_image(std::vector<glm::vec3> &image, std::string file_name, uint32_t width, uint32_t height) {
    std::ofstream ofs; 
    
    ofs.open(file_name, std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    
    for (size_t i = 0; i < height * width; ++i) {
        glm::vec3 &c = image[i];

        float max = std::max(c[0], std::max(c[1], c[2]));
        
        if (max > 1) c = c * (1.f / max);
        
        for (size_t j = 0; j < 3; j++) {
            ofs << (char)(255 * std::max(0.f, std::min(1.f, image[i][j])));
        }
    }
    ofs.close();
}
