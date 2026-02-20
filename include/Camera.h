#pragma once

#include <glm/glm.hpp>
#include <Window.h>

class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 direction = glm::vec3(0, -90.0f, 0), float fov = 70.0f, glm::vec3 backgroundColor = glm::vec3(0.0f, 0.0f, 0.0f));
    Camera(float posX, float posY, float posZ, float roll, float pitch, float yaw, float fov = 70.0f, glm::vec3 backgroundColor = glm::vec3(0.0f, 0.0f, 0.0f));

    glm::vec3 Front() const;
    glm::vec3 Right() const;
    glm::vec3 Up() const;

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    glm::vec3 position;
    glm::vec3 direction;
    float fov;
    glm::vec3 backgroundColor;

private:
    Window& _window;
};