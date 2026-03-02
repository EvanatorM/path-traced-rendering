#pragma once

#include <scenes/SceneObject.h>
#include <rendering/render-objects/Shader.h>

#pragma pack(1)
struct GPUCube
{
    glm::vec4 position;
    glm::vec4 size;
    glm::vec4 color;
};
#pragma pack()

struct Cube : public SceneObject
{
    glm::vec3 size;

    Cube(const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& size = glm::vec3(1.0f), const glm::vec3& col = glm::vec3(1.0f))
        : SceneObject(pos, col), size(size) {}

    void RenderRaster(Shader& shader) const;

    GPUCube GetGPUCube() const
    {
        return { glm::vec4(position, 0.0f), glm::vec4(size, 0.0f), { color.r, color.g, color.b, 1.0f } };
    }
};