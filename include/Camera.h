#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 direction = glm::vec3(0.0f, 0.0f, -90.0f), glm::vec3 backgroundColor = glm::vec3(0.0f), float fov = 70.0f) 
    : position(position), direction(direction), backgroundColor(backgroundColor), fov(fov) {}

    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 backgroundColor;
    float fov;
};