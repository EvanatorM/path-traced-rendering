#pragma once

#include <SceneObject.h>

#pragma pack(1)
struct GPUSphere
{
    glm::vec3 center;
    float radius;
    glm::vec4 color;
};
#pragma pack()

struct Sphere : public SceneObject
{
    float radius;

    Sphere(const glm::vec3& pos, const glm::vec3& col, float r)
        : SceneObject(pos, col), radius(r) {}
    Sphere(float r)
        : SceneObject(), radius(r) {}
    Sphere()
        : SceneObject(), radius(1.0f) {}

    GPUSphere GetGPUSphere() const
    {
        return { position, radius, { color.r, color.g, color.b, 1.0f } };
    }
};