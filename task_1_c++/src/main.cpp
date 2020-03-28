#include <cmath>
#include <limits>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "objects.h"
#include "antialiasting.h"

uint32_t threads_num = 8;
std::string out_file = "./out.ppm";
std::vector<glm::vec3> env;
int32_t env_width, env_height;
glm::vec3 back_color(0.1, 0.1, 0.1);

inline float norm(const glm::vec3 &now) {
    return std::sqrt(glm::dot(now, now));
}


inline glm::vec3 reflect(const glm::vec3 &I, const glm::vec3 &N) {
    return I - N * 2.f * (glm::dot(I,N));
}


glm::vec3 refract(const  glm::vec3 &I, const glm::vec3 &N, const float eta_t, const float eta_i=1.f) { // Snell's law
    float cosi = -glm::dot(I, N);

    if (cosi < 0) return refract(I, -N, eta_i, eta_t);
    
    float eta = eta_i / eta_t;
    float k = 1 - eta * eta * (1 - cosi * cosi);

    return k < 0 ? glm::vec3(1,0,0) : I*eta + N * (eta * cosi - std::sqrt(k));
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


glm::vec3 cast_ray(const glm::vec3 &orig, const glm::vec3 &dir, const std::vector<Object *> &objects, const std::vector<Light> &lights, size_t depth) {
    glm::vec3 point, N;
    Material material;

    if (depth <= 0 || !scene_intersect(orig, dir, objects, point, N, material)) {
        return back_color;
    }

    glm::vec3 reflect_dir   = reflect(dir, N);
    glm::vec3 refract_dir   = refract(dir, N, material.refractive_index);
    
    glm::vec3 reflect_color = cast_ray(glm::dot(reflect_dir, N) < 0 ? point - N * 1e-3f : point + N * 1e-3f, reflect_dir, objects, lights, depth - 1);
    glm::vec3 refract_color = cast_ray(glm::dot(refract_dir, N) < 0 ? point - N * 1e-3f : point + N * 1e-3f, refract_dir, objects, lights, depth - 1);

    float diffuse_light_intensity = 0;
    float specular_light_intensity = 0;

    for (size_t i = 0; i < lights.size(); i++) {
        glm::vec3 light_dir  = glm::normalize(lights[i].position - point);

        glm::vec3 shadow_orig = glm::dot(light_dir,N) < 0 ? point - N * 1e-3f : point + N * 1e-3f; // checking if the point lies in the shadow of the lights[i]
        glm::vec3 shadow_pt, shadow_N;

        Material tmpmaterial;
        if (scene_intersect(shadow_orig, light_dir, objects, shadow_pt, shadow_N, tmpmaterial) && norm(shadow_pt - shadow_orig) < norm(lights[i].position - point))
            continue;

        diffuse_light_intensity  += lights[i].intensity * std::max(0.f, glm::dot(light_dir , N));
        specular_light_intensity += std::pow(std::max(0.f, glm::dot(-reflect(-light_dir, N),dir)), material.specular_exponent) * lights[i].intensity;
    }

    return material.diffuse_color * diffuse_light_intensity  * material.albedo[0] + 
           glm::vec3(1., 1., 1.)  * specular_light_intensity * material.albedo[1] + 
           reflect_color          * material.albedo[2] + 
           refract_color          * material.albedo[3];
}

void print_image(std::vector<glm::vec3> &image, std::string file_name, uint32_t width, uint32_t height) {
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


glm::vec3 normalize_color(const glm::vec3 &now) {
    return glm::vec3(std::max(0.f, std::min(1.f, now[0])), std::max(0.f, std::min(1.f, now[1])), std::max(0.f, std::min(1.f, now[2])));
}


void render(const std::vector<Object *> &objects, const std::vector<Light> &lights) {
    const int   width    = 1920;
    const int   height   = 1080;
    const float fov      = M_PI/3.;
    const glm::vec3 camera(0.2, 0.0, 0.3);

    std::vector<glm::vec3> image(width * height);

    #pragma omp parallel for num_threads(threads_num)
    for (size_t j = 0; j < height; j++) { // actual rendering loop
        for (size_t i = 0; i < width; i++) {
            
            float dir_x =  (i + 0.5) -width  / 2.;
            float dir_y = -(j + 0.5) +height / 2.; // this flips the image at the same time
            float dir_z = -height / (2. * std::tan(fov /2.));

            image[i + j * width] = normalize_color(cast_ray(camera, glm::normalize(glm::vec3(dir_x, dir_y, dir_z)), objects, lights, 4));
        }
    }

    std::vector<glm::vec3> edges(image.size());
    detect_image_edges(image, edges, width, height, threads_num);
    print_image(edges, "./out_edg.ppm", width, height);

    #pragma omp parallel for num_threads(threads_num)
    for (int x = 1; x < width - 1; x++) {
        for (int y = 1; y < height - 1; y++) {
            float gray = edges[x + y * width][0];
            
            if(gray > 0.02) {
                float t_x =  x - width  / 2.0;
                float t_y = -y + height / 2.0;
                float dir_z = -height / (2. * std::tan(fov /2.));

                auto c = image[x + y * width];
                
                float weight = 1.0 / 4;
                c = c * weight;
                c += cast_ray(camera, glm::normalize(glm::vec3(t_x + 0.5, t_y, dir_z)), objects, lights, 4) * weight;
                c += cast_ray(camera, glm::normalize(glm::vec3(t_x, t_y + 0.5, dir_z)), objects, lights, 4) * weight;
                c += cast_ray(camera, glm::normalize(glm::vec3(t_x + 0.5, t_y + 0.5, dir_z)), objects, lights, 4) * weight;
                image[x + y * width] = normalize_color(c);
            }
        }
    }
    print_image(image, out_file, width, height);
}


void first_scene() {
    Material      ivory(1.0, glm::vec4(0.6,  0.3, 0.1, 0.0), glm::vec3(0.4, 0.4, 0.3),   50.);
    Material      glass(1.5, glm::vec4(0.1,  1.0, 0.1, 0.8), glm::vec3(0.6, 0.7, 0.8),  140.);
    Material red_rubber(1.0, glm::vec4(0.9,  0.1, 0.0, 0.0), glm::vec3(0.3, 0.1, 0.1),   10.);
    Material ivory_blue(1.0, glm::vec4(0.6,  0.3, 0.1, 0.0), glm::vec3(0.1, 0.1, 0.6),   50.);
    Material     mirror(1.0, glm::vec4(0.4, 10.0, 0.8, 0.0), glm::vec3( .0,  .8,  .0), 1425.);
    Material  an_mirror(1.0, glm::vec4(0.9, 10.0, 0.8, 0.2), glm::vec3(  0,   0,   0),  200.);
    
    std::vector<Object *> objects;
    objects.push_back(new Sphere(glm::vec3(-3,    0,   -16),  2,      ivory));
    objects.push_back(new Sphere(glm::vec3(-10,  -1.5, -12),  2,      glass));
    objects.push_back(new Cube  (glm::vec3( 10,  -1.5, -25),  5, red_rubber));
    objects.push_back(new Cube  (glm::vec3( 10,  -1.5,  -7),  5, ivory_blue));
    objects.push_back(new Sphere(glm::vec3( 7,    5,   -30),  4,     mirror));
    objects.push_back(new Plane (glm::vec3( 0,    1,     0), -4,  an_mirror, glm::vec3(.2, .2, .2), glm::vec3(0., 0., 0.), glm::vec3(1000, 1000, 1000), true));
    

    std::vector<Light>  lights;
    lights.push_back(Light(glm::vec3(-20, 20,  20), 1.5));
    lights.push_back(Light(glm::vec3( 30, 50, -25), 2.5));
    lights.push_back(Light(glm::vec3( 30, 20,  30), 1.7));
    
    render(objects, lights);
}

void second_scene() {
    Material      ivory(1.0, glm::vec4(0.6,  0.3, 0.1, 0.0), glm::vec3(0.4, 0.4, 0.3),   50.);
    Material      glass(1.5, glm::vec4(0.1,  1.0, 0.1, 0.8), glm::vec3(0.6, 0.7, 0.8),  140.);
    Material  an_mirror(1.0, glm::vec4(0.9, 10.0, 0.3, 0.2), glm::vec3(  0,   0,   0),  2.000);
    Material red_rubber(1.0, glm::vec4(0.3,  0.5, 0.2, 0.0), glm::vec3(0.3, 0.1, 0.1),   10.);
    Material ivory_blue(1.0, glm::vec4(0.6,  0.3, 0.1, 0.0), glm::vec3(0.75, 0.25, 0.25),   50.);
    Material     rubber(1.0, glm::vec4(0.5,  0.1, 0.0, 0.0), glm::vec3(0.3, 0.1, 0.1),   10.);
    
    std::vector<Object *> objects;
    objects.push_back(new Sphere(glm::vec3(-1, -1.5,   -18),   2,  ivory));
    objects.push_back(new Cube  (glm::vec3( -5,-3.25,   -15), 1.5,  ivory_blue));
    objects.push_back(new Plane (glm::vec3( 0,    1,     0),  -4,      rubber, glm::vec3(0. , 0. , 0. ), glm::vec3(.75, .75, .75), glm::vec3(10, 20, 30), false)); //bottom
    objects.push_back(new Plane (glm::vec3( 1,    0,     0), -10,  red_rubber, glm::vec3(0. , 0. , 0. ), glm::vec3(.25, .25, .75), glm::vec3(10, 10, 30), false)); //left
    objects.push_back(new Plane (glm::vec3( 0,    0,     1), -30,      rubber, glm::vec3(0. , 0. , 0. ), glm::vec3(.75, .75, .75), glm::vec3(10, 10, 30), false)); // back
    objects.push_back(new Plane (glm::vec3(-1,    0,     0),  10,  red_rubber, glm::vec3(0. , 0. , 0. ), glm::vec3(.75, .25, .25), glm::vec3(10, 10, 30), false)); //right
    objects.push_back(new Plane (glm::vec3( 0,    1,     0),  10,  red_rubber, glm::vec3(0. , 0. , 0. ), glm::vec3(.75, .75, .75), glm::vec3(10, 20, 30), false)); //top
    
    std::vector<Light>  lights;
    lights.push_back(Light(glm::vec3(0, 8,  -20), 1.5));
    //lights.push_back(Light(glm::vec3( 30, 50, -25), 2.5));
    lights.push_back(Light(glm::vec3(0, 7, 0), 1.7));

    render(objects, lights);
}

void fird_scene() {

}


int main(int argc, char **argv) {
    std::map<std::string, std::string> cmd_prams;
    
    for (int i = 0; i < argc; i++) {
        std::string now(argv[i]);
        if (now.size() and now[0] == '-') {
            cmd_prams[now] = i == argc - 1 ? "" : argv[i + 1];
        }
    }

    if (cmd_prams.find("-out") != cmd_prams.end()) {
        out_file = cmd_prams["-out"];
    }
    
    if (cmd_prams.find("-threads") != cmd_prams.end()) {
        threads_num = std::atoi(cmd_prams["-threads"].c_str());
    }

    uint32_t scene_id = 1;
    if (cmd_prams.find("-scene") != cmd_prams.end()) {
        scene_id = std::atoi(cmd_prams["-scene"].c_str());
    }

    if (scene_id == 1) first_scene();
    if (scene_id == 2) second_scene();
    if (scene_id == 3) fird_scene();

    return 0;
}

