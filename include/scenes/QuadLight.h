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
    float padding1;
    glm::vec3 color;
    float padding2;
};
#pragma pack()

struct QuadLight : public SceneObject
{
    glm::vec3 u, v;
    float intensity;
    float attenuation;

    QuadLight(const glm::vec3& pos = glm::vec3(0.0f),  const glm::vec3& u = glm::vec3(1.0f, 0.0f, 0.0f), const glm::vec3& v = glm::vec3(0.0f, 1.0f, 0.0f), const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f, float attenuation = 1.0f)
        : SceneObject(pos, color), u(u), v(v), intensity(intensity), attenuation(attenuation) {}

    GPUQuadLight GetGPUQuadLight() const
    {
        return { position, intensity, u, attenuation, v, 0.0f, color, 0.0f };

    }
};