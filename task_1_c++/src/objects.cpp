#include "objects.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

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


void Sphere::process(float &all_dist, glm::vec3 &hit, glm::vec3 &N, Material &material, float &dist_i, const glm::vec3 &orig, const glm::vec3 &dir) {
    all_dist = dist_i;
    hit = orig + dir*dist_i;
    N = glm::normalize(hit - center);
    material = this->material;
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


void Cube::process(float &all_dist, glm::vec3 &hit, glm::vec3 &N, Material &material, float &dist_i, const glm::vec3 &orig, const glm::vec3 &dir) {
    all_dist = dist_i;
    hit = orig + dir*dist_i;
    
    glm::vec3 point = hit - center;
    
    float min = std::numeric_limits<float>::max();
    float distance = std::abs(radius - std::abs(point.x));
    if (distance < min) {
        min = distance;
        N = glm::vec3(1, 0, 0);
        N *= point.x < 0 ? -1 : 1;
    }
    distance = std::abs(radius - std::abs(point.y));
    if (distance < min) {
        min = distance;
        N = glm::vec3(0, 1, 0);
        N *= point.y < 0 ? -1 : 1;
    }
    distance = std::abs(radius - std::abs(point.z));
    if (distance < min) { 
        min = distance; 
        N = glm::vec3(0, 0, 1);
        N *= point.z < 0 ? -1 : 1;
    } 
    material = this->material;
}


bool Plane::ray_intersect(const glm::vec3 &orig, const glm::vec3 &dir, float &t0) const {
    float d = -(orig[dist] - radius) / dir[dist]; 
    glm::vec3 pt = orig + dir * d;

    if (d > 0 and std::abs(pt[fir]) < size[fir] and pt[sec] < size[sec] and pt[sec] > -size[sec]) {
        if (std::abs(dir[dist]) > 1e-3f) {
            t0 = d;
            return true;
        }
    }
    return false;
}


void Plane::process(float &all_dist, glm::vec3 &hit, glm::vec3 &N, Material &material, float &dist_i, const glm::vec3 &orig, const glm::vec3 &dir) {
    if (dist_i >= all_dist) return;
    else all_dist = dist_i;
    hit = orig + dir * dist_i; 
    N   = center;
    material = this->material;
    material.diffuse_color = (int(.5 * hit[fir] + 1000) + int(.5 * hit[sec])) & 1 and ecen ? color_1 : color_2; 
}
