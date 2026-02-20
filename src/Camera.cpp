#include <Camera.h>

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 direction, float fov, glm::vec3 backgroundColor)
    : _window(Window::GetInstance()), position(position), direction(direction), fov(fov), backgroundColor(backgroundColor)
{

}

// constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float roll, float pitch, float yaw, float fov, glm::vec3 backgroundColor)
    : _window(Window::GetInstance()), fov(fov), backgroundColor(backgroundColor)
{
    position = glm::vec3(posX, posY, posZ);
    direction = glm::vec3(pitch, yaw, roll);
}

glm::vec3 Camera::Front() const
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(direction.y)) * cos(glm::radians(direction.x));
    front.y = sin(glm::radians(direction.x));
    front.z = sin(glm::radians(direction.y)) * cos(glm::radians(direction.x));
    return glm::normalize(front);
}

glm::vec3 Camera::Right() const
{
    return glm::normalize(glm::cross(Front(), glm::vec3(0, 1, 0)));
}

glm::vec3 Camera::Up() const
{
    return glm::normalize(glm::cross(Right(), Front()));
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, position + Front(), Up());
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    glm::ivec2 w = _window.GetWindowSize();
    return glm::perspective(glm::radians(fov), (float)w.x / (float)w.y, 0.1f, 1000.0f);
}