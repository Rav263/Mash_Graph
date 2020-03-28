#include "shaders/shader.h"
#include <vector>

Shader::Shader(std::string shader_path, GLenum shader_type) {
    this->shader_path = shader_path;
    this->shader_source = new ShaderSource(shader_path);
    compile_shader(shader_type);
}

void Shader::compile_shader(GLenum shader_type) {
    this->shader = glCreateShader(shader_type);
    glShaderSource(this->shader, 1, &this->shader_source->shader_source, NULL);
    glCompileShader(this->shader);

    // check for shader compile errors
    int success;
    glGetShaderiv(this->shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(this->shader, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n IN SHADER: " << this->shader_path 
            << std::endl
            << info_log << std::endl;
        exit(1);
    }
}

uint32_t Shader::get_shader() {
    return this->shader;
}

Shader::~Shader() {
    glDeleteShader(this->shader);
    delete this->shader_source;
}


ShaderProgram::ShaderProgram(int argc, std::vector<Shader *> shaders) {
    this->shaders = shaders;
    link_shader_program();
}

void ShaderProgram::link_shader_program() {
    this->shader_program = glCreateProgram();
    
    for (Shader *shader : shaders) {
        glAttachShader(this->shader_program, shader->get_shader());
    }
    glLinkProgram(this->shader_program);

    // check for linking errors
    int success;
    glGetProgramiv(this->shader_program, GL_LINK_STATUS, &success);
    
    if (!success){
        char info_log[512];
        glGetProgramInfoLog(this->shader_program, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
        exit(1);
    }
}

uint32_t ShaderProgram::get_shader_program() {
    return this->shader_program;
}

ShaderProgram::~ShaderProgram() {
    for (auto shader : this->shaders) {
        delete shader;
    }
    glDeleteProgram(this->shader_program);
}
