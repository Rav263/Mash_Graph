#include <limits>
#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "objects.h"
#include "scene.h"


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
        const std::vector<Object *> &objects, const std::vector<Light> &lights, float &diffuse_intensity, float &specular_intensity) {
    
    for (size_t i = 0; i < lights.size(); i++) {
        glm::vec3 light_dir  = glm::normalize(lights[i].position - point);

        glm::vec3 shadow_orig = glm::dot(light_dir,N) < 0 ? point - N * 1e-3f : point + N * 1e-3f;
        glm::vec3 shadow_pt, shadow_N;

        Material tmpmaterial;
        if (scene_intersect(shadow_orig, light_dir, objects, shadow_pt, shadow_N, tmpmaterial) && glm::length(shadow_pt - shadow_orig) < glm::length(lights[i].position - point))
            continue;

        diffuse_intensity  += lights[i].intensity * std::max(0.f, glm::dot(light_dir , N));
        specular_intensity += std::pow(std::max(0.f, glm::dot(-glm::reflect(-light_dir, N), dir)), material.specular_exponent) * lights[i].intensity;
    }
}


glm::vec3 cast_ray(const glm::vec3 &orig, const glm::vec3 &dir, const std::vector<Object *> &objects, const std::vector<Light> &lights, size_t depth) {
    glm::vec3 point, N;
    Material material;

    if (depth <= 0 || !scene_intersect(orig, dir, objects, point, N, material)) return back_color;

    glm::vec3 reflect_dir = glm::reflect(dir, N);
    glm::vec3 refract_dir = refract(dir, N, material.refractive_index); 

    float diffuse_intensity = 0;
    float specular_intensity = 0;

    calculate_light_intensity(N, point, dir, material, objects, lights, diffuse_intensity, specular_intensity); 
   
    glm::vec3 reflect_color = cast_ray(glm::dot(reflect_dir, N) < 0 ? point - N * 1e-3f : point + N * 1e-3f, reflect_dir, objects, lights, depth - 1);
    glm::vec3 refract_color = cast_ray(glm::dot(refract_dir, N) < 0 ? point - N * 1e-3f : point + N * 1e-3f, refract_dir, objects, lights, depth - 1);

    return material.diffuse_color * diffuse_intensity  * material.diff_coof + 
           glm::vec3(1., 1., 1.)  * specular_intensity * material.spec_coof + 
           reflect_color          * material.reflect_coof + 
           refract_color          * material.refract_coof;
}
