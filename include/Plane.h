#pragma once

#include <SceneObject.h>

struct Plane : public SceneObject
{
    glm::vec3 orientation;

    Plane(const glm::vec3& pos, const glm::vec3& orientation, const glm::vec3& col)
        : SceneObject(pos, col), orientation(glm::normalize(orientation)) {}
    Plane()
        : SceneObject(), orientation(glm::vec3(0.0f)) {}

    // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
    bool Intersect(const Ray& ray, float& t) const override
    {
        float denom = glm::dot(orientation, ray.direction);
        if (denom > 1e-6)
        {
            glm::vec3 p010 = position - ray.origin;
            t = glm::dot(p010, orientation) / denom;
            return (t >= 0);
        }

        return false;
    }
};