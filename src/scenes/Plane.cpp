#include <scenes/Plane.h>

#include <rendering/render-objects/Mesh.h>
#include <glm/gtc/matrix_transform.hpp>

void Plane::RenderRaster(Shader& shader) const
{
    static Vertex vertices[] = {
        // positions   // normals    // texCoords
        glm::vec3(-100.0f, 0.0f,  100.0f), glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(0.0f, 1.0f),
        glm::vec3( 100.0f, 0.0f,  100.0f), glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(1.0f, 1.0f),
        glm::vec3(-100.0f, 0.0f, -100.0f), glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(0.0f, 0.0f),
        glm::vec3( 100.0f, 0.0f, -100.0f), glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(1.0f, 0.0f)
    };
    static uint32_t indices[] = {
        0, 2, 1,
        1, 2, 3
    };
    static Mesh mesh(vertices, 4, indices, 6);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(orientation.x), glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(orientation.y), glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, glm::radians(orientation.z), glm::vec3(0.0, 0.0, 1.0));
    shader.SetMat4("model", model);

    mesh.Draw();
}

bool Plane::OverlapRay(const Ray& ray, float& dist) const
{
    float denom = glm::dot(orientation, ray.direction);
    if (denom > 1e-6)
    {
        glm::vec3 p010 = position - ray.origin;
        dist = glm::dot(p010, orientation) / denom;

        if (dist < 0) return false;

        return true;
    }

    return false;
}