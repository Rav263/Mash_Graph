#ifndef SCENE
#define SCENE


#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "objects.h"


static uint32_t threads_num = 8;
static std::string out_file = "./out.bpm";
static glm::vec3 back_color(0.1, 0.1, 0.1);



glm::vec3 cast_ray(const glm::vec3 &orig, const glm::vec3 &dir, const std::vector<Object *> &objects, const std::vector<Light> &lights, size_t depth);


#endif
