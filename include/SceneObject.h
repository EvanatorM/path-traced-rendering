#pragma once

#include <Ray.h>
#include <glm/glm.hpp>

struct SceneObject
{
    glm::vec3 position;
    glm::vec3 color;

    SceneObject() : position(0.0f), color(1.0f) {}
    SceneObject(const glm::vec3& pos, const glm::vec3& col) : position(pos), color(col) {}
};