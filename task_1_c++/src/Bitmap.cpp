#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <cstring>
#include "Bitmap.h"


void WriteBMP(const char* fname, Pixel* a_pixelData, int width, int height) {
    int paddedsize = (width*height) * sizeof(Pixel);

    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};

    bmpfileheader[ 2] = (unsigned char)(paddedsize    );
    bmpfileheader[ 3] = (unsigned char)(paddedsize>> 8);
    bmpfileheader[ 4] = (unsigned char)(paddedsize>>16);
    bmpfileheader[ 5] = (unsigned char)(paddedsize>>24);

    bmpinfoheader[ 4] = (unsigned char)(width    );
    bmpinfoheader[ 5] = (unsigned char)(width>> 8);
    bmpinfoheader[ 6] = (unsigned char)(width>>16);
    bmpinfoheader[ 7] = (unsigned char)(width>>24);
    bmpinfoheader[ 8] = (unsigned char)(height    );
    bmpinfoheader[ 9] = (unsigned char)(height>> 8);
    bmpinfoheader[10] = (unsigned char)(height>>16);
    bmpinfoheader[11] = (unsigned char)(height>>24);

    std::ofstream out(fname, std::ios::out | std::ios::binary);
    out.write((const char*)bmpfileheader, 14);
    out.write((const char*)bmpinfoheader, 40);
    out.write((const char*)a_pixelData, paddedsize);
    out.flush();
    out.close();
}

void print_image(std::vector<glm::vec3> &image, std::string file_name, uint32_t w, uint32_t h) {
    std::vector<Pixel> pixels2(w * h);

    for (size_t i = 0; i < pixels2.size(); i++) {
        Pixel px;
        glm::vec3 &c = image[i];

        float max = std::max(c[0], std::max(c[1], c[2]));
        
        if (max > 1) c = c * (1.f / max);

        px.r       = c[2] * 255;
        px.g       = c[1] * 255;
        px.b       = c[0] * 255;
        pixels2[i] = px;
    }

    WriteBMP(file_name.c_str(), &pixels2[0], w, h);
}
