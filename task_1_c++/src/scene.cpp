#include <limits>
#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "objects.h"
#include "scene.h"

inline float norm(const glm::vec3 &now) {
    return std::sqrt(glm::dot(now, now));
}


inline glm::vec3 reflect(const glm::vec3 &I, const glm::vec3 &N) {
    return I - N * 2.f * glm::dot(I,N);
}


glm::vec3 refract(const  glm::vec3 &I, const glm::vec3 &N, const float eta_t, const float eta_i=1.f) {
    auto cosi = -glm::dot(I, N);

    if (cosi < 0) return refract(I, -N, eta_i, eta_t);
    
    auto eta = eta_i / eta_t;
    auto k = 1 - eta * eta * (1 - cosi * cosi);

    return k < 0 ? glm::vec3(1,0,0) : I * eta + N * (eta * cosi - std::sqrt(k));
}


bool scene_intersect(const glm::vec3 &orig, const glm::vec3 &dir, const std::vector<Object *> &objects, glm::vec3 &hit, glm::vec3 &N, Material &material) {
    float all_dist = std::numeric_limits<float>::max();

    for (size_t i = 0; i < objects.size(); i++) {
        float dist_i;

        if (objects[i]->ray_intersect(orig, dir, dist_i) && dist_i < all_dist) {
            objects[i]->process(all_dist, hit, N, material, dist_i, orig, dir);
        }
    }

    return all_dist < 1000;
}


void calculate_light_intensity(glm::vec3 &N, glm::vec3 &point, const glm::vec3 &dir, Material &material,
        const std::vector<Object *> &objects, const std::vector<Light> &lights, float &diffuse_intensity, float &specular_intensity){
    
    for (size_t i = 0; i < lights.size(); i++) {
        glm::vec3 light_dir  = glm::normalize(lights[i].position - point);

        glm::vec3 shadow_orig = glm::dot(light_dir,N) < 0 ? point - N * 1e-3f : point + N * 1e-3f;
        glm::vec3 shadow_pt, shadow_N;

        Material tmpmaterial;
        if (scene_intersect(shadow_orig, light_dir, objects, shadow_pt, shadow_N, tmpmaterial) && norm(shadow_pt - shadow_orig) < norm(lights[i].position - point))
            continue;

        diffuse_intensity  += lights[i].intensity * std::max(0.f, glm::dot(light_dir , N));
        specular_intensity += std::pow(std::max(0.f, glm::dot(-reflect(-light_dir, N), dir)), material.specular_exponent) * lights[i].intensity;
    }
}


glm::vec3 cast_ray(const glm::vec3 &orig, const glm::vec3 &dir, const std::vector<Object *> &objects, const std::vector<Light> &lights, size_t depth) {
    glm::vec3 point, N;
    Material material;

    if (depth <= 0 || !scene_intersect(orig, dir, objects, point, N, material)) return back_color;

    glm::vec3 reflect_dir = reflect(dir, N);
    glm::vec3 refract_dir = refract(dir, N, material.refractive_index); 

    float diffuse_intensity = 0;
    float specular_intensity = 0;

    calculate_light_intensity(N, point, dir, material, objects, lights, diffuse_intensity, specular_intensity); 
    
    glm::vec3 reflect_color = cast_ray(glm::dot(reflect_dir, N) < 0 ? point - N * 1e-3f : point + N * 1e-3f, reflect_dir, objects, lights, depth - 1);
    glm::vec3 refract_color = cast_ray(glm::dot(refract_dir, N) < 0 ? point - N * 1e-3f : point + N * 1e-3f, refract_dir, objects, lights, depth - 1);

    return material.diffuse_color * diffuse_intensity  * material.albedo[0] + 
           glm::vec3(1., 1., 1.)  * specular_intensity * material.albedo[1] + 
           reflect_color          * material.albedo[2] + 
           refract_color          * material.albedo[3];
}
