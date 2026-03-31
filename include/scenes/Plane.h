#pragma once

#include <scenes/SceneObject.h>
#include <rendering/render-objects/Shader.h>

#pragma pack(1)
struct GPUPlane
{
    glm::vec3 offset;
    uint32_t matIndex;
    glm::vec3 orientation;
    float padding;
};
#pragma pack()

struct Plane : public SceneObject
{
    glm::vec3 orientation;

    Plane(const glm::vec3& pos, const glm::vec3& orientation, uint32_t materialIndex = 0)
        : SceneObject(pos, materialIndex), orientation(glm::normalize(orientation)) {}
    Plane()
        : SceneObject(), orientation(glm::vec3(0.0f)) {}

    void RenderRaster(Shader& shader) const;

    GPUPlane GetGPUPlane() const
    {
        return { position, materialIndex, orientation, 0.0f };
    }
    
    bool OverlapRay(const Ray& ray, float& dist) const override;
};