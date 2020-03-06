#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "buffers.h"
#include "vertices.h"
#include "indices.h"

#include <iostream>
#include <vector>
#include <tuple>

std::tuple<uint32_t, uint32_t, uint32_t> create_buffers() {
    uint32_t VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO); 
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return std::make_tuple(VAO, VBO, EBO);
}
