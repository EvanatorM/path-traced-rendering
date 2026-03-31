#pragma once

#include <rendering/Ray.h>
#include <glm/glm.hpp>

struct SceneObject
{
    glm::vec3 position;
    uint32_t materialIndex;

    SceneObject() : position(0.0f), materialIndex(0) {}
    SceneObject(const glm::vec3& pos, uint32_t materialIndex) : position(pos), materialIndex(materialIndex) {}

    virtual bool OverlapRay(const Ray& ray, float& dist) const { return false; };
};