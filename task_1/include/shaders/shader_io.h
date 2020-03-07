#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#ifndef SHADER_IO
#define SHADER_IO

class ShaderSource {
    std::string shader_path;
public:
    char *shader_source;
    ShaderSource(std::string);
    ~ShaderSource() {
        delete this->shader_source;
        std::cerr << "deletind shader_source: " << this->shader_path << std::endl;
    }

    const char *get_shader_source();
    void read_shader_source();
    std::string get_shader_path() {
        return this->shader_path;
    }
};  

#endif
