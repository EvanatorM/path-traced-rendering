#pragma once

#include <scenes/SceneObject.h>
#include <rendering/render-objects/Shader.h>

#pragma pack(1)
struct GPUPlane
{
    glm::vec3 offset;
    float padding1;
    glm::vec3 orientation;
    float padding2;
    glm::vec4 color;
};
#pragma pack()

struct Plane : public SceneObject
{
    glm::vec3 orientation;

    Plane(const glm::vec3& pos, const glm::vec3& orientation, const glm::vec3& col)
        : SceneObject(pos, col), orientation(glm::normalize(orientation)) {}
    Plane()
        : SceneObject(), orientation(glm::vec3(0.0f)) {}

    void RenderRaster(Shader& shader) const;

    GPUPlane GetGPUPlane() const
    {
        return { position, 0.0f, orientation, 0.0f, { color.r, color.g, color.b, 1.0f } };
    }
};