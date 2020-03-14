#version 330 core

layout (location = 0) in vec3 position;

out vec3 some_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 color;
uniform vec3 location;

void main() {
    gl_Position = projection * view * model * vec4(position + location, 1.0f);
    some_color = color;
}
