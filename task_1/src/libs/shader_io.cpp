#include "shaders/shader_io.h"

#include <exception>


ShaderSource::ShaderSource(std::string shader_path) {
    this->shader_path = shader_path;
    this->read_shader_source();
}

void ShaderSource::read_shader_source() {
    std::string shader_source;
    try {
        std::fstream shader_file(this->shader_path);
        std::stringstream shader_stream;

        shader_stream << shader_file.rdbuf();

        shader_file.close();

        shader_source = shader_stream.str();

    } catch(std::exception &e) {
        std::cerr << "FALED TO LOAD SHADER FILE: " << this->shader_path << std::endl 
                  << "With error: " << e.what() << std::endl;
        exit(1);
    }
    this->shader_source = new char[shader_source.size() + 1]; 
    int i = 0;

    for (auto ch : shader_source) {
        this->shader_source[i++] = ch;
    }
    this->shader_source[shader_source.size()] = '\0';
    //std::cerr << this->shader_source << std::endl;
}

const char *ShaderSource::get_shader_source(){
    return this->shader_source;
}
