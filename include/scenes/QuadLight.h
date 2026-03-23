#pragma once

#include <scenes/SceneObject.h>

#pragma pack(1)
struct GPUQuadLight
{
    glm::vec3 position;
    float intensity;
    glm::vec3 u;
    float attenuation;
    glm::vec3 v;
    uint32_t matIndex;
    glm::vec3 color;
    float area;
};
#pragma pack()

struct QuadLight : public SceneObject
{
    glm::vec3 u, v;
    glm::vec3 color;
    float intensity;
    float attenuation;

    QuadLight(const glm::vec3& pos = glm::vec3(0.0f),  const glm::vec3& u = glm::vec3(1.0f, 0.0f, 0.0f), const glm::vec3& v = glm::vec3(0.0f, 1.0f, 0.0f), uint32_t materialIndex = 0, const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f, float attenuation = 1.0f)
        : SceneObject(pos, materialIndex), color(color), u(u), v(v), intensity(intensity), attenuation(attenuation) {}

    GPUQuadLight GetGPUQuadLight() const
    {
        return { position, intensity, u, attenuation, v, materialIndex, color, 1.0f };

    }
};