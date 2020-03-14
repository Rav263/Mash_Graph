#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "libs/object.h"
#include "libs/buffers.h"

#include <vector>
#include <fstream>
#include <tuple>

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>


Object::Object(std::string name, uint32_t shader_program) {
    this->shader_program = shader_program;
    this->name = name;
    read_file();
    this->buffer = std::get<0>(create_buffers(this->arrays));
}

void Object::read_file(){
    std::ifstream vertices_file(this->name+".vrt");
    std::vector<float> verts;

    float x, y, z;
    for(int i; vertices_file >> x >> y >> z; i++) {
       verts.push_back(x);
       verts.push_back(y);
       verts.push_back(z);
    }
    vertices_file.close();

    this->arrays.vertices = new float[verts.size()];
    this->arrays.vertices_size = verts.size();

    for (int i = 0; i < verts.size(); i++) {
        this->arrays.vertices[i] = verts[i];
    }

    uint32_t a, b, c;
    std::ifstream indices_file(this->name+".ind");
    std::vector<uint32_t> indices;

    for (int i = 0; indices_file >> a >> b >> c; i++) {
        indices.push_back(a);
        indices.push_back(b);
        indices.push_back(c);
    }

    this->arrays.indices = new uint32_t[indices.size()];
    this->arrays.indices_size = indices.size();

    for (int i = 0; i < indices.size(); i++) {
        this->arrays.indices[i] = indices[i];
    }
}

void Object::draw_object(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection) {
    GLint model_loc = glGetUniformLocation(this->shader_program, "model");
    GLint view_loc = glGetUniformLocation(this->shader_program, "view");
    GLint proj_loc = glGetUniformLocation(this->shader_program, "projection");

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));


    // draw triangle
    glBindVertexArray(this->buffer);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Object::~Object() {
    delete[] this->arrays.vertices;
    delete[] this->arrays.indices;
}
