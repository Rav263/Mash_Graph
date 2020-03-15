#include <iostream>
#include <vector>
#include <tuple>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include "Bitmap.h"


int HEIGH = 600;
int WIDTH = 600;

float view_width = 1;
float view_heigh = 1;
float projection_z = 1;

float inf = 10000000;

//glm::mat3 camera_rotation = glm::mat3(0.7071f, 0.f, -0.7071f, 0.f, 1.0f, 0.f, 0.7071f, 0.f, 0.7071f);
glm::mat3 camera_rotation = glm::rotate(glm::mat3(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f), glm::radians(0.0f));

class Object {
public:
    glm::vec3 center;
    float radius;
    glm::vec3 color;
    float specular;
    float reflective;

    Object(glm::vec3 center, float radius, glm::vec3 color, float specular, float reflective) {
        this->center = center;
        this->radius = radius;
        this->color = color;
        this->specular = specular;
        this->reflective = reflective;
    }
};

struct Light {
    uint32_t type;
    float intensity;
    glm::vec3 position;

    Light(uint32_t type, float intensity, glm::vec3 position) {
        this->type = type;
        this->intensity = intensity;
        this->position = position;
    }
};

glm::vec3 canvas_to_view(int x, int y) {
    return glm::vec3((float)x * (view_width / WIDTH), (float)y * (view_heigh / HEIGH), projection_z);
}

std::pair<float, float> intersect_ray_sphere(glm::vec3 origin, glm::vec3 direction, Object * object) {
    auto oc = origin - object->center;
    
    auto k1 = glm::dot(direction, direction);
    auto k2 = 2 * glm::dot(oc, direction);
    auto k3 = glm::dot(oc, oc) - object->radius * object->radius;

    auto dicr = k2 * k2 - 4 * k1 * k3;

    if (dicr < 0) {
        return {inf, inf};
    }

    return {(-k2 + std::sqrt(dicr)) / (2 * k1), (-k2 - std::sqrt(dicr)) / (2 * k1)};
}

std::pair<Object *, float> closest_intersection(glm::vec3 origin, glm::vec3 direction, float min_t, float max_t, std::vector<Object *> &objects) {
    float closest_t = inf;
    Object *closest_sphere = nullptr;

    for (auto object : objects) {
        auto ts = intersect_ray_sphere(origin, direction, object);

        if (ts.first < closest_t and min_t < ts.first and ts.first < max_t) {
            closest_t = ts.first;
            closest_sphere = object;
        }

        if (ts.second < closest_t and min_t < ts.second and ts.second < max_t) {
            closest_t = ts.second;
            closest_sphere = object;
        }
    }

    return {closest_sphere, closest_t};
}


float compute_lightning(glm::vec3 point, glm::vec3 normal, glm::vec3 view, float specular, std::vector<Light *> &lights, std::vector<Object *> objects) {
    float intensity = 0;
    glm::vec3 vec_l, vec_r;
    float t_max;

    for (auto light : lights) {
        if (light->type == 0) { // AMBIENT
            intensity = light->intensity;
        } else if (light->type == 1) { // POINT
            vec_l = light->position - point;
            t_max = 1.0;
        } else {
            vec_l = light->position;
            t_max = inf;
        }

        auto blocker = closest_intersection(point, vec_l, 0.001, t_max, objects);
        if (blocker.first != nullptr) {
            continue;
        }

        auto n_dot_l = glm::dot(normal, vec_l);
        if (n_dot_l > 0) {
            intensity += light->intensity * n_dot_l / (glm::length(normal) * glm::length(vec_l));
        }

        if (specular != -1) {
            auto vec_r = glm::reflect(vec_l, normal);
            auto r_dot_v = glm::dot(vec_r, view);

            if (r_dot_v > 0) {
                intensity += light->intensity * std::pow(r_dot_v / (glm::length(vec_r) * glm::length(view)), specular);
            }
        }
    }

    return intensity;
}

glm::vec3 ray_trace(glm::vec3 origin, glm::vec3 direction, float min_t, float max_t, uint32_t depth, std::vector<Object *> &objects, std::vector<Light *> &lights) {
    auto intersection = closest_intersection(origin, direction, min_t, max_t, objects);
    if (intersection.first == nullptr) {
        return {0.f, 0.f, 0.f};
    }

    auto point = origin + (direction * intersection.second);
    auto normal = point - intersection.first->center;

    normal = (1.0f / glm::length(normal)) * normal;

    auto view = -1.f * direction;
    auto lightning = compute_lightning(point, normal, view, intersection.first->specular, lights, objects);
    auto local_color = lightning * intersection.first->color;

    if (intersection.first->reflective <= 0 or depth <= 0) {
        return local_color;
    }

    auto reflected_ray = glm::reflect(view, normal);
    auto reflected_color = ray_trace(point, reflected_ray, 0.001, inf, depth - 1, objects, lights);

    return ((1 - intersection.first->reflective) * local_color) + (intersection.first->reflective * reflected_color);
}
uint32_t normalize(int32_t a) {
    if (a < 0) return 0;
    if (a > 255) return 255;
    return a;
}

void put_pixel(int x, int y, glm::vec3 color, uint32_t *pixels) {
    int x_t = WIDTH / 2 + x;
    int y_t = HEIGH / 2 + y;


    int offset = x_t + WIDTH * y_t;
    pixels[offset] += normalize(std::floor(color[2]));
    pixels[offset] <<= 8;
    pixels[offset] += normalize(std::floor(color[1]));
    pixels[offset] <<= 8;
    pixels[offset] += normalize(std::floor(color[0]));
}

int main() {
    std::vector<Object *> objects;
    objects.push_back(new Object({0.f, -1.f, 3.f}, 1, {255, 0, 0}, 500, 0.2));
    objects.push_back(new Object({2.f, 0.f, 4.f}, 1, {0, 0, 255}, 500, 0.3));
    objects.push_back(new Object({-2.f, 0.f, 4.f}, 1, {0, 255, 0}, 10, 0.4));
    objects.push_back(new Object({0.f, -5001.f, 0.f}, 5000, {255, 255, 0}, 1000, 0.5));
   
    std::vector<Light *> lights;
    lights.push_back(new Light(0, 0.2, {0,0,0}));
    lights.push_back(new Light(1, 0.6, {2, 1, 0}));
    lights.push_back(new Light(2, 0.2, {1, 4, 4}));
    uint32_t *pixels = new uint32_t[WIDTH * HEIGH];

    for (int x = -WIDTH / 2; x < WIDTH / 2; x++) {
        for (int y = -HEIGH / 2; y < HEIGH / 2; y++) {
            auto direction = camera_rotation * canvas_to_view(x, y);
            auto color = ray_trace({0.f, 0.f, -1.f}, direction, 1.0, inf, 4, objects, lights);
            put_pixel(x, y, color, pixels);
        }
    }
    std::cout << std::endl;

    SaveBMP("rectangle.bmp", pixels, WIDTH, HEIGH);
}
