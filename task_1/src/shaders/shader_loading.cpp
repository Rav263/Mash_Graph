#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <tuple>

const char *vertex_shader_source = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";


const char *fragment_shader_source = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

uint32_t compile_vertex_shader() {
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    // check for shader compile errors
    int success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
    }
    return vertex_shader; 
}


uint32_t compile_fragment_shader() {
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    
    // check for shader compile errors
    int success;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
    }
    return fragment_shader;
}

uint32_t link_shader_program(uint32_t vertex_shader, uint32_t fragment_shader) {
    uint32_t shader_program= glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // check for linking errors
    int success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    
    if (!success){
        char info_log[512];
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
    }
    return shader_program;
}

uint32_t create_shader_program() {
     // build and compile our shader program
    auto vertex_shader = compile_vertex_shader();
    auto fragment_shader = compile_fragment_shader();
    
    // link shaders
    auto shader_program = link_shader_program(vertex_shader, fragment_shader);   

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}
