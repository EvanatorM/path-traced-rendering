#pragma once

#include <scenes/SceneObject.h>
#include <rendering/render-objects/Shader.h>

#pragma pack(1)
struct GPUCube
{
    glm::vec4 position;
    glm::vec3 size;
    uint32_t matIndex;
};
#pragma pack()

struct Cube : public SceneObject
{
    glm::vec3 size;

    Cube(const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& size = glm::vec3(1.0f), uint32_t materialIndex = 0)
        : SceneObject(pos, materialIndex), size(size) {}

    void RenderRaster(Shader& shader) const;

    GPUCube GetGPUCube() const
    {
        return { glm::vec4(position, 0.0f), size, materialIndex };
    }
    
    bool OverlapRay(const Ray& ray, float& dist) const override;
};