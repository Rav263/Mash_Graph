#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "BMP/Bitmap.h"
#include "shaders/shader_creation.h"
#include "libs/init_opengl.h"
#include "libs/buffers.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>
#include <tuple>


void render_image(uint32_t shader_program, uint32_t buffer) {
    float vertices[] = {
         0.5f,  0.5f, 0.0f, // top right
         0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f, // top left 
    };  

    glUseProgram(shader_program);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 model(1.f);
    glm::mat4 view(1.f);
    glm::mat4 projection(1.f);

    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

    glm::mat4 some = projection * view * model;

    std::cerr << "MODEL: " << std::endl << glm::to_string(model) << std::endl;
    std::cerr << "VIEW: "  << std::endl << glm::to_string(view) << std::endl;
    std::cerr << "PROJ: "  << std::endl << glm::to_string(projection) << std::endl;
    std::cerr << "MULT: "  << std::endl << glm::to_string(some) << std::endl;
   
    std::cerr << "VERTICES: " << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cerr << glm::to_string(model * glm::vec4(vertices[i*3], vertices[i*3 + 1], vertices[i*3 + 2], 1.0f)) << std::endl;
    }

    GLint model_loc = glGetUniformLocation(shader_program, "model");
    GLint view_loc = glGetUniformLocation(shader_program, "view");
    GLint proj_loc = glGetUniformLocation(shader_program, "projection");

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));


    // draw triangle
    glBindVertexArray(buffer);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

int main() {
    //init window and glfw
    auto window = init();
   
    //init shader program
    auto shader_program = create_shader_program();

    // set up vertex buffers and configure vertex attributes 
    auto buffers = create_buffers();

    // render image
    render_image(shader_program->get_shader_program(), std::get<0>(buffers));

    // read pixels and save to BMP
    unsigned int *image = new unsigned[SCR_WIDTH * SCR_HEIGHT];

    glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SaveBMP("rectangle.bmp", image, SCR_WIDTH, SCR_HEIGHT);

    delete[] image;

    // clean everything
    glDeleteVertexArrays(1, &std::get<0>(buffers));
    glDeleteBuffers(1, &std::get<1>(buffers));
    glDeleteBuffers(1, &std::get<2>(buffers));
    delete shader_program;

    glfwTerminate();
    return 0;
}
