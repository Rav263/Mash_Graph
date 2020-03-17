#ifndef OBJECTS_H
#define OBJECTS_H

#include <glm/glm.hpp>

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
};

class Cube : public Object {
public:
    Cube(const glm::vec3 &c, const float r, const Material &m) : Object(c, r, m) {}
    virtual bool ray_intersect(const glm::vec3 &orig, const glm::vec3 &dir, float &t0) const;
};

class Sphere : public Object {
public:
    Sphere(const glm::vec3 &c, const float r, const Material &m) : Object(c, r, m) {}
    virtual bool ray_intersect(const glm::vec3 &orig, const glm::vec3 &dir, float &t0) const;
};

#endif
