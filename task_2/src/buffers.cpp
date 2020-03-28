#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "libs/buffers.h"
#include "libs/object.h"
#include "data/cube_indices.h"


#include <iostream>
#include <vector>
#include <tuple>


std::tuple<uint32_t, uint32_t, uint32_t> create_buffers(Object_arrays &arrays) {
    uint32_t VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO); 
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, arrays.vertices_size * sizeof(float), arrays.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, arrays.indices_size * sizeof(uint32_t), arrays.indices, GL_STATIC_DRAW);
    
    //atribute for vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    
    //atribute for color
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return std::make_tuple(VAO, VBO, EBO);
}
