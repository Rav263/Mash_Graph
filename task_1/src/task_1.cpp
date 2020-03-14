#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "BMP/Bitmap.h"
#include "shaders/shader_creation.h"
#include "libs/init_opengl.h"
#include "libs/buffers.h"
#include "libs/object.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>
#include <tuple>


void render_image(uint32_t shader_program, std::vector<Object *> &objects) {
    glUseProgram(shader_program);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model(1.f);
    glm::mat4 view(1.f);
    glm::mat4 projection(1.f);

    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

    for (auto object : objects) {
        object->draw_object(model, view, projection);
    }
}

int main() {
    //init window and glfw
    auto window = init();
   
    //init shader program
    auto shader_program = create_shader_program();

    // set up vertex buffers and configure vertex attributes 
    Object *cube = new Object("./objects/cube", 
                              shader_program->get_shader_program(), 
                              glm::vec3(1.0f, 1.0f, 1.0f),
                              glm::vec3(0.4f, 0.f, 0.f));
    
    std::vector<Object *> objects;

    objects.push_back(cube);

    // render image
    render_image(shader_program->get_shader_program(), objects);

    // read pixels and save to BMP
    unsigned int *image = new unsigned[SCR_WIDTH * SCR_HEIGHT];

    glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SaveBMP("rectangle.bmp", image, SCR_WIDTH, SCR_HEIGHT);

    delete[] image;

    // clean everything
    
    /* Need to move into object class
     * glDeleteVertexArrays(1, &std::get<0>(buffers));
     * glDeleteBuffers(1, &std::get<1>(buffers));
     * glDeleteBuffers(1, &std::get<2>(buffers));
    */
    delete shader_program;

    glfwTerminate();
    return 0;
}
