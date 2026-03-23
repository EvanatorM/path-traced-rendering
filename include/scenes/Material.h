#pragma once

#include <glm/glm.hpp>

struct GPUMaterial
{
    glm::vec4 albedo_roughness;
    glm::vec4 emission_metallic;
};

struct Material
{
    glm::vec3 albedo;
    glm::vec3 emission;
    float roughness;
    float metallic;

    Material(const glm::vec3& albedo, const glm::vec3& emission, float roughness, float metallic)
        : albedo(albedo), emission(emission), roughness(roughness), metallic(metallic) {}

    GPUMaterial GetGPUMaterial() const
    {
        return { glm::vec4(albedo, roughness), glm::vec4(emission, metallic) };
    }
};