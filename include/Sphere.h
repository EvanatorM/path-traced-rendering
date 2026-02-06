#pragma once

#include <SceneObject.h>

struct Sphere : public SceneObject
{
    float radius;

    Sphere(const glm::vec3& pos, const glm::vec3& col, float r)
        : SceneObject(pos, col), radius(r) {}
    Sphere(float r)
        : SceneObject(), radius(r) {}
    Sphere()
        : SceneObject(), radius(1.0f) {}

    // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
    bool Intersect(const Ray& ray, float& t) const override
    {
        float t0, t1;

        glm::vec3 L = position - ray.origin;
        float tca = glm::dot(L, ray.direction);
        if (tca < 0) return false;
        float d2 = glm::dot(L, L) - tca * tca;
        if (d2 > radius * radius) return false;
        float thc = sqrt(radius * radius - d2);
        t0 = tca - thc;
        t1 = tca + thc;

        if (t0 > t1) std::swap(t0, t1);

        if (t0 < 0)
        {
            t0 = t1; 
            if (t0 < 0) return false; 
        }

        t = t0;
        
        return true;
    }
};