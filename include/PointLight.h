#pragma once

#include <SceneObject.h>

#pragma pack(1)
struct GPUPointLight
{
    glm::vec4 position;

    float constant;
    float linear;
    float quadratic;
    float padding1;

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};
#pragma pack()

struct PointLight : public SceneObject
{
    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    PointLight(const glm::vec3& pos = glm::vec3(0.0f), float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f,
               const glm::vec3& ambient = glm::vec3(0.1f), const glm::vec3& diffuse = glm::vec3(0.8f), const glm::vec3& specular = glm::vec3(1.0f))
        : SceneObject(pos, glm::vec3(0.0f)), constant(constant), linear(linear), quadratic(quadratic), ambient(ambient), diffuse(diffuse), specular(specular) {}

    GPUPointLight GetGPUPointLight() const
    {
        return { glm::vec4(position, 1.0f), constant, linear, quadratic, 0.0f, glm::vec4(ambient, 1.0f), glm::vec4(diffuse, 1.0f), glm::vec4(specular, 1.0f) };
    }
};