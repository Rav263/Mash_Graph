#include <cmath>
#include <limits>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "objects.h"
#include "antialiasting.h"

float norm(const glm::vec3 &now) {
    return std::sqrt(glm::dot(now, now));
}


glm::vec3 reflect(const glm::vec3 &I, const glm::vec3 &N) {
    return I - N * 2.f * (glm::dot(I,N));
}


glm::vec3 refract(const  glm::vec3 &I, const glm::vec3 &N, const float eta_t, const float eta_i=1.f) { // Snell's law
    float cosi = -std::max(-1.f, std::min(1.f, glm::dot(I,N)));

    if (cosi < 0)
        return refract(I, -N, eta_i, eta_t); // if the ray comes from the inside the object, swap the air and the media
    
    float eta = eta_i / eta_t;
    float k = 1 - eta * eta * (1 - cosi * cosi);

    return k < 0 ? glm::vec3(1,0,0) : I*eta + N * (eta * cosi - std::sqrt(k)); // k<0 = total reflection, no ray to refract. I refract it anyways, this has no physical meaning
}


bool scene_intersect(const glm::vec3 &orig, const glm::vec3 &dir, const std::vector<Object *> &objects, glm::vec3 &hit, glm::vec3 &N, Material &material) {
    float spheres_dist = std::numeric_limits<float>::max();

    for (size_t i = 0; i < objects.size(); i++) {
        float dist_i;

        if (objects[i]->ray_intersect(orig, dir, dist_i) && dist_i < spheres_dist) {
            spheres_dist = dist_i;
            hit = orig + dir*dist_i;
            N = glm::normalize(hit - objects[i]->center);
            material = objects[i]->material;
        }
    }

    float checkerboard_dist = std::numeric_limits<float>::max();
    
    if (std::abs(dir.y) > 1e-3f)  {
        float d = -(orig.y + 4) / dir.y; // the checkerboard plane has equation y = -4
        glm::vec3 pt = orig + dir * d;

        if (d > 0 and std::abs(pt.x) < 20 and pt.z < 0 and pt.z > -50 and d < spheres_dist) {
            checkerboard_dist = d;
            hit = pt;
            N = glm::vec3(0,1,0);
            material.diffuse_color = (int(.5 * hit.x + 1000) + int(.5 * hit.z)) & 1 ? glm::vec3(.3, .3, .3) : glm::vec3(.3, .2, .1);
        }
    }
    return std::min(spheres_dist, checkerboard_dist) < 100;
}


glm::vec3 cast_ray(const glm::vec3 &orig, const glm::vec3 &dir, const std::vector<Object *> &objects, const std::vector<Light> &lights, size_t depth) {
    glm::vec3 point, N;
    Material material;

    if (depth <= 0 || !scene_intersect(orig, dir, objects, point, N, material)) {
        return glm::vec3(0.2, 0.7, 0.8); // background color
    }

    glm::vec3 reflect_dir   = glm::normalize(reflect(dir, N));
    glm::vec3 refract_dir   = glm::normalize(refract(dir, N, material.refractive_index));
    glm::vec3 reflect_orig  = glm::dot(reflect_dir, N) < 0 ? point - N * 1e-3f : point + N * 1e-3f; // offset the original point to avoid occlusion by the object itself
    glm::vec3 refract_orig  = glm::dot(refract_dir, N) < 0 ? point - N * 1e-3f : point + N * 1e-3f;
    glm::vec3 reflect_color = cast_ray(reflect_orig, reflect_dir, objects, lights, depth - 1);
    glm::vec3 refract_color = cast_ray(refract_orig, refract_dir, objects, lights, depth - 1);

    float diffuse_light_intensity = 0;
    float specular_light_intensity = 0;

    for (size_t i = 0; i < lights.size(); i++) {
        glm::vec3 light_dir  = glm::normalize(lights[i].position - point);
        float light_distance = norm(lights[i].position - point);

        glm::vec3 shadow_orig = glm::dot(light_dir,N) < 0 ? point - N * 1e-3f : point + N * 1e-3f; // checking if the point lies in the shadow of the lights[i]
        glm::vec3 shadow_pt, shadow_N;

        Material tmpmaterial;
        
        if (scene_intersect(shadow_orig, light_dir, objects, shadow_pt, shadow_N, tmpmaterial) and norm(shadow_pt-shadow_orig) < light_distance)
            continue;

        diffuse_light_intensity  += lights[i].intensity * std::max(0.f, glm::dot(light_dir , N));
        specular_light_intensity += powf(std::max(0.f, glm::dot(-reflect(-light_dir, N),dir)), material.specular_exponent) * lights[i].intensity;
    }

    return material.diffuse_color * diffuse_light_intensity  * material.albedo[0] + 
           glm::vec3(1., 1., 1.)  * specular_light_intensity * material.albedo[1] + 
           reflect_color          * material.albedo[2] + 
           refract_color          * material.albedo[3];
}

void print_image(std::vector<glm::vec3> &image, std::string file_name, uint32_t width, uint32_t height) {
    // save the framebuffer to file

    std::ofstream ofs; 
    
    ofs.open(file_name, std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    
    for (size_t i = 0; i < height * width; ++i) {
        glm::vec3 &c = image[i];

        float max = std::max(c[0], std::max(c[1], c[2]));
        
        if (max > 1) c = c * (1.f / max);
        
        for (size_t j = 0; j < 3; j++) {
            ofs << (char)(255 * std::max(0.f, std::min(1.f, image[i][j])));
        }
    }
    ofs.close();
}


void render(const std::vector<Object *> &objects, const std::vector<Light> &lights) {
    const int   width    = 1920;
    const int   height   = 1080;
    const float fov      = M_PI/3.;

    std::vector<glm::vec3> image(width * height);

    #pragma omp parallel for
    for (size_t j = 0; j < height; j++) { // actual rendering loop
        for (size_t i = 0; i < width; i++) {
            
            float dir_x =  (i + 0.5) -width  / 2.;
            float dir_y = -(j + 0.5) +height / 2.; // this flips the image at the same time
            float dir_z = -height / (2. * std::tan(fov /2.));

            image[i + j * width] = cast_ray(glm::vec3(0,0,0), glm::normalize(glm::vec3(dir_x, dir_y, dir_z)), objects, lights, 4);
        }
    }

    print_image(image, "./out.ppm", width, height);

    std::vector<glm::vec3> edges(image.size());
    detect_image_edges(image, edges, width, height);


    for (int x = 1; x < width - 1; x++) {
        for (int y = 1; y < height - 1; y++) {
            // edges canvas is grayscaled
            // it means that color components (r, g, b) are equal
            float gray = edges[x + y * width][0];
            
            // TODO: improve
            if(gray > 0.04) {
                float t_x =  x - width / 2.0;
                float t_y = -y +  height / 2.0;
                float dir_z = -height / (2. * std::tan(fov /2.));

                auto c = image[x + y * width];
                
                float weight = 1.0 / 4;
                c = c * weight;
                c += cast_ray({0, 0, 0}, glm::normalize(glm::vec3(t_x + 0.5, t_y, dir_z)), objects, lights, 6) * weight;
                c += cast_ray({0, 0, 0}, glm::normalize(glm::vec3(t_x, t_y + 0.5, dir_z)), objects, lights, 6) * weight;
                c += cast_ray({0, 0, 0}, glm::normalize(glm::vec3(t_x + 0.5, t_y + 0.5, dir_z)), objects, lights, 6) * weight;
                image[x + y * width] = c;
            }
        }
    }
    print_image(edges, "./out_gray.ppm", width, height);
    print_image(image, "./out_ant.ppm", width, height);
}


int main() {
    Material      ivory(1.0, glm::vec4(0.6,  0.3, 0.1, 0.0), glm::vec3(0.4, 0.4, 0.3),   50.);
    Material      glass(1.5, glm::vec4(0.0,  0.5, 0.1, 0.8), glm::vec3(0.6, 0.7, 0.8),  125.);
    Material red_rubber(1.0, glm::vec4(0.9,  0.1, 0.0, 0.0), glm::vec3(0.3, 0.1, 0.1),   10.);
    Material     mirror(1.0, glm::vec4(0.0, 10.0, 0.8, 0.0), glm::vec3(1.0, 1.0, 1.0), 1425.);

    std::vector<Object *> objects;
    objects.push_back(new Sphere(glm::vec3(-3,    0,   -16), 2,      ivory));
    objects.push_back(new Sphere(glm::vec3(-1.0, -1.5, -12), 2,      glass));
    objects.push_back(new Sphere(glm::vec3( 1.5, -0.5, -18), 3, red_rubber));
    objects.push_back(new Sphere(glm::vec3( 7,    5,   -18), 4,     mirror));

    

    std::vector<Light>  lights;
    lights.push_back(Light(glm::vec3(-20, 20,  20), 1.5));
    lights.push_back(Light(glm::vec3( 30, 50, -25), 1.8));
    lights.push_back(Light(glm::vec3( 30, 20,  30), 1.7));

    render(objects, lights);

    return 0;
}

