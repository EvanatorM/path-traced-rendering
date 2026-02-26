#pragma once

#include <SceneObject.h>

#pragma pack(1)
struct GPUPointLight
{
    glm::vec3 position;
    float attenuation;
    glm::vec3 color;
    float intensity;
};
#pragma pack()

struct PointLight : public SceneObject
{
    float intensity;
    float attenuation;

    PointLight(const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f, float attenuation = 1.0f)
        : SceneObject(pos, color), intensity(intensity), attenuation(attenuation) {}

    GPUPointLight GetGPUPointLight() const
    {
        return { position, attenuation, color, intensity };
    }
};