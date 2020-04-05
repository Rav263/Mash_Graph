#ifndef BITMAP_GUARDIAN_H
#define BITMAP_GUARDIAN_H

struct Pixel { unsigned char r, g, b; };

void print_image(std::vector<glm::vec3> &image, std::string file_name, uint32_t w, uint32_t h);

#endif 
