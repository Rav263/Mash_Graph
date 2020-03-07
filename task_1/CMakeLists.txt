cmake_minimum_required(VERSION 3.8)
project(rt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")

add_subdirectory(external/glfw-3.3.2)
add_subdirectory(external/glm)
include_directories(include)

set(LIBS glfw X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread glm)

add_library(GLAD src/libs/glad.c)
set(LIBS ${LIBS} GLAD)

add_executable(rt src/task_1.cpp src/libs/Bitmap.cpp src/libs/create_shaders.cpp src/init_opengl.cpp src/buffers.cpp src/libs/shader_io.cpp src/libs/shader.cpp)

target_link_libraries(rt ${LIBS})

file(GLOB SHADERS src/shaders/*.glsl)
foreach(SHADER ${SHADERS})
    file(COPY ${SHADER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)
endforeach(SHADER)
