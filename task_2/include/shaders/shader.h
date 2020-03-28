#include "shader_io.h"
#include <vector>

#ifndef SHADER
#define SHADER

class Shader {
    ShaderSource *shader_source;
    std::string shader_path;
    uint32_t shader;
    void compile_shader(GLenum shaderType);
public:
    Shader(std::string, GLenum);
    ~Shader();

    uint32_t get_shader();
};

class ShaderProgram {
    std::vector<Shader *> shaders;
    uint32_t shader_program;
    
    void link_shader_program();
public:
    ShaderProgram(int argc, std::vector<Shader *> shaders);
    ~ShaderProgram();

    uint32_t get_shader_program();
};

#endif
