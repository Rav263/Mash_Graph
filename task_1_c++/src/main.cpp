#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "objects.h"
#include "antialiasting.h"
#include "file_io.h"
#include "scene.h"


glm::vec3 normalize_color(const glm::vec3 &now) {
    return glm::vec3(std::max(0.f, std::min(1.f, now[0])), std::max(0.f, std::min(1.f, now[1])), std::max(0.f, std::min(1.f, now[2])));
}


void render(const std::vector<Object *> &objects, const std::vector<Light> &lights) {
    const int   width    = 1024; 
    const int   height   = 600;

    const float fov      = M_PI / 3.;
    const glm::vec3 camera(0.2, 0.0, 0.3);

    std::vector<glm::vec3> image(width * height);

    #pragma omp parallel for num_threads(threads_num)
    for (int32_t y = 0; y < height; y++) { 
        for (int32_t x = 0; x < width; x++) {
            
            float dir_x =  (x + 0.5) -width  / 2.;
            float dir_y = -(y + 0.5) +height / 2.; // this flips the image at the same time
            float dir_z = -height / (2. * std::tan(fov / 2.));

            image[x + y * width] = normalize_color(cast_ray(camera, glm::normalize(glm::vec3(dir_x, dir_y, dir_z)), objects, lights, 4));
        }
    }

    std::vector<glm::vec3> edges(image.size());
    detect_image_edges(image, edges, width, height, threads_num);
    //print_image(edges, "./out_edg.ppm", width, height);

    #pragma omp parallel for num_threads(threads_num)
    for (int32_t x = 1; x < width - 1; x++) {
        for (int32_t y = 1; y < height - 1; y++) {
            float gray = edges[x + y * width][0];
            
            if(gray > 0.2) {
                float t_x =  x - width  / 2.0;
                float t_y = -y + height / 2.0;
                
                float dir_z = -height / (2. * std::tan(fov / 2.));

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
    lights.push_back(Light(glm::vec3(0, 7, 0), 1.7));

    render(objects, lights);
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

    return 0;
}

