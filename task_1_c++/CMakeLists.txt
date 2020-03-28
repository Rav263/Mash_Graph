cmake_minimum_required(VERSION 3.8)
project(rt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O5 -fopenmp")

add_subdirectory(external/glm)

include_directories(include)

set(LIBS glm)

add_executable(rt src/main.cpp src/objects.cpp src/antialiasting.cpp src/file_io.cpp src/scene.cpp)

target_link_libraries(rt ${LIBS})

file(GLOB ENVMAPS env/*.ppm)
foreach(ENVMAP ${ENVMAPS})
    file(COPY ${ENVMAP} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/env)
endforeach(ENVMAP)

