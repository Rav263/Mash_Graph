#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <glm/glm.hpp>


#ifndef OBJECT
#define OBJECT

struct Object_arrays {
    float *vertices;
    uint32_t *indices;
    size_t vertices_size;
    size_t indices_size;
};

class Object {
    Object_arrays arrays;
    glm::vec3 color;
    std::string name;
    glm::vec3 position;
    uint32_t shader_program;
    uint32_t buffer;

public:
    Object(std::string, uint32_t, glm::vec3, glm::vec3);
    ~Object();

    void read_file();
    void draw_object(glm::mat4 &, glm::mat4 &, glm::mat4 &);
};

#endif
