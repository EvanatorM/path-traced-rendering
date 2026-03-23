#pragma once

#include <scenes/SceneObject.h>
#include <rendering/render-objects/Shader.h>

#pragma pack(1)
struct GPUSphere
{
    glm::vec3 center;
    float radius;
    uint32_t matIndex;
    glm::vec3 padding;
};
#pragma pack()

struct Sphere : public SceneObject
{
    float radius;

    Sphere(const glm::vec3& pos, uint32_t materialIndex, float r)
        : SceneObject(pos, materialIndex), radius(r) {}
    Sphere(float r)
        : SceneObject(), radius(r) {}
    Sphere()
        : SceneObject(), radius(1.0f) {}

    void RenderRaster(Shader& shader) const;

    GPUSphere GetGPUSphere() const
    {
        return { position, radius, materialIndex, glm::vec3(0.0f) };
    }
};