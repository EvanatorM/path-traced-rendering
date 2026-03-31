#pragma once

#include <scenes/SceneObject.h>
#include <rendering/render-objects/Shader.h>

#pragma pack(1)
struct GPUSphere
{
    glm::vec3 center;
    float radius;
    glm::vec3 padding;
    uint32_t matIndex;
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
        return { position, radius, glm::vec3(0.0f), materialIndex };
    }

    bool OverlapRay(const Ray& ray, float& dist) const override;
};