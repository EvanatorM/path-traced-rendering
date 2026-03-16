#pragma once

#include <scenes/SceneObject.h>

#pragma pack(1)
struct GPUAreaSphereLight
{
    glm::vec3 position;
    float attenuation;
    glm::vec3 color;
    float intensity;
    glm::vec3 padding;
    float radius;
};
#pragma pack()

struct AreaSphereLight : public SceneObject
{
    float intensity;
    float attenuation;
    float radius;

    AreaSphereLight(const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f, float attenuation = 1.0f, float radius = 1.0f)
        : SceneObject(pos, color), intensity(intensity), attenuation(attenuation), radius(radius) {}

    GPUAreaSphereLight GetGPUAreaSphereLight() const
    {
        return { position, attenuation, color, intensity, glm::vec3(0.0f), radius };
    }
};