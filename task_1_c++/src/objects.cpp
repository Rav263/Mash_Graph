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
    glm::vec3 invdir = 1.0f / dir;
    glm::vec3 sign((float)(invdir.x < 0), (float)(invdir.y < 0), (float)(invdir.z < 0));

    auto tmin  = (bounds[    sign[0]].x - orig.x) * invdir.x; 
    auto tmax  = (bounds[1 - sign[0]].x - orig.x) * invdir.x; 
    auto tymin = (bounds[    sign[1]].y - orig.y) * invdir.y; 
    auto tymax = (bounds[1 - sign[1]].y - orig.y) * invdir.y; 
 
    if ((tmin > tymax) || (tymin > tmax)) return false; 
    if (tymin > tmin) tmin = tymin; 
    if (tymax < tmax) tmax = tymax; 
 
    auto tzmin = (bounds[    sign[2]].z - orig.z) * invdir.z; 
    auto tzmax = (bounds[1 - sign[2]].z - orig.z) * invdir.z; 
 
    if ((tmin > tzmax) || (tzmin > tmax)) return false; 
    if (tzmin > tmin) tmin = tzmin; 
    if (tzmax < tmax) tmax = tzmax; 
    
    t0 = tmin; 
 
    if (t0 < 0) { 
        t0 = tmax; 
        if (t0 < 0) return false; 
    } 
 
    return true; 
} 
