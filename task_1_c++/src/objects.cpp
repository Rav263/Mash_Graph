#include "objects.h"
#include <glm/glm.hpp>


bool Sphere::ray_intersect(const glm::vec3 &orig, const glm::vec3 &dir, float &t0) const {
    glm::vec3 L = center - orig;
    float tca   = glm::dot(L, dir);
    float d2    = glm::dot(L, L) - tca * tca;

    if (d2 > radius * radius)
        return false;

    float thc = std::sqrt(radius * radius - d2);
    
    t0       = tca - thc;
    float t1 = tca + thc;

    if (t0 < 0) t0 = t1;
    if (t0 < 0) return false;
    return true;
}

bool Cube::ray_intersect(const glm::vec3 &orig, const glm::vec3 &dir, float &t0) const {
   return glm::dot(dir,dir) > radius;
}
