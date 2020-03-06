#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "BMP/Bitmap.h"
#include "shader_loading.h"
#include "init_opengl.h"
#include "buffers.h"


#include <iostream>
#include <vector>
#include <tuple>


void render_image(uint32_t shader_program, uint32_t buffer) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw triangle
    glUseProgram(shader_program);
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
    render_image(shader_program, std::get<0>(buffers));

    // read pixels and save to BMP
    unsigned int *image = new unsigned[SCR_WIDTH * SCR_HEIGHT];

    glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SaveBMP("rectangle.bmp", image, SCR_WIDTH, SCR_HEIGHT);

    delete[] image;

    // clean everything
    glDeleteVertexArrays(1, &std::get<0>(buffers));
    glDeleteBuffers(1, &std::get<1>(buffers));
    glDeleteBuffers(1, &std::get<2>(buffers));

    glfwTerminate();
    return 0;
}
