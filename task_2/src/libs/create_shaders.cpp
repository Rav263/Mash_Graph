#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaders/shader_io.h"
#include "shaders/shader.h"

#include <iostream>
#include <vector>
#include <tuple>

std::string VERTEX_SHADER_PATH = "./shaders/vertex.glsl";
std::string FRAGMENT_SHADER_PATH = "./shaders/fragment.glsl";

ShaderProgram *create_shader_program() {
    // read shader source and build shaders
    Shader *vertex_shader   = new Shader(VERTEX_SHADER_PATH, GL_VERTEX_SHADER);
    Shader *fragment_shader = new Shader(FRAGMENT_SHADER_PATH, GL_FRAGMENT_SHADER);

    // create shader program and link shaders
    auto shader_program = new ShaderProgram(2, {vertex_shader, fragment_shader});   

    return shader_program;
}
