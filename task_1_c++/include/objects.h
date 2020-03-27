#ifndef OBJECTS_H
#define OBJECTS_H

#include <glm/glm.hpp>
#include <vector>

struct Light {
    Light(const glm::vec3 &p, const float i)
        : position(p), intensity(i) {}
    
    glm::vec3 position;
    float intensity;
};

struct Material {
    Material(const float r, const glm::vec4 &a, const glm::vec3 &color, const float spec) 
        : refractive_index(r), albedo(a), diffuse_color(color), specular_exponent(spec) {}
    
    Material() 
        : refractive_index(1), albedo({1,0,0,0}), diffuse_color(), specular_exponent() {}
    
    float refractive_index;
    glm::vec4 albedo;
    glm::vec3 diffuse_color;
    float specular_exponent;
};

class Object {
public:
    glm::vec3 center;
    float radius;
    Material material;
    
    Object(const glm::vec3 &c, const float r, const Material &m)
        : center(c), radius(r), material(m) {}
    
    virtual bool ray_intersect(const glm::vec3 &orig, const glm::vec3 &dir, float &t0) const = 0;
    virtual void process(float &all_dist, glm::vec3 &hit, glm::vec3 &N, Material &material, float &dist_i, const glm::vec3 &orig, const glm::vec3 &dir) = 0;
};

class Cube : public Object {
public:
    std::vector<glm::vec3> bounds;
    Cube(const glm::vec3 &c, const float r, const Material &m) : Object(c, r, m) {
        float half = r / 2.0;
        bounds.push_back(c - half);
        bounds.push_back(c + half);
    }
    virtual bool ray_intersect(const glm::vec3 &orig, const glm::vec3 &dir, float &t0) const;
    virtual void process(float &all_dist, glm::vec3 &hit, glm::vec3 &N, Material &material, float &dist_i, const glm::vec3 &orig, const glm::vec3 &dir);
};

class Sphere : public Object {
public:
    Sphere(const glm::vec3 &c, const float r, const Material &m) : Object(c, r, m) {}
    virtual bool ray_intersect(const glm::vec3 &orig, const glm::vec3 &dir, float &t0) const;
    virtual void process(float &all_dist, glm::vec3 &hit, glm::vec3 &N, Material &material, float &dist_i, const glm::vec3 &orig, const glm::vec3 &dir);
};


class Plane : public Object {
public:
    uint32_t dist;
    glm::vec3 color_1;
    glm::vec3 color_2;
    bool ecen;
    glm::vec3 size;
    uint32_t fir, sec;
    Plane(const glm::vec3 &c, const float r, const Material &m, const glm::vec3 &color_1, const glm::vec3 &color_2, const glm::vec3 &size, bool ecen) : Object(c, r, m) {
        if      (c[0]) {this->dist = 0; this->fir = 1; this->sec = 2;}
        else if (c[1]) {this->dist = 1; this->fir = 0; this->sec = 2;}
        else           {this->dist = 2; this->fir = 0; this->sec = 1;}
        this->color_1 = color_1;
        this->color_2 = color_2;
        this->ecen    = ecen;
        this->size    = size;
    }
    virtual bool ray_intersect(const glm::vec3 &orig, const glm::vec3 &dir, float &t0) const;
    virtual void process(float &all_dist, glm::vec3 &hit, glm::vec3 &N, Material &material, float &dist_i, const glm::vec3 &orig, const glm::vec3 &dir);
};


#endif
