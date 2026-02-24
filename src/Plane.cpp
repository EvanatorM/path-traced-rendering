#include <Plane.h>

#include <Mesh.h>
#include <Shader.h>
#include <glm/gtc/matrix_transform.hpp>

void Plane::RenderRaster(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& viewPos) const
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

    static Shader shader("assets/shaders/shape.vert", "assets/shaders/shape.frag");

    shader.Bind();
    shader.SetMat4("view", view);
    shader.SetMat4("proj", proj);
    shader.SetVec3("viewPos", viewPos);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(orientation.x), glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(orientation.y), glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, glm::radians(orientation.z), glm::vec3(0.0, 0.0, 1.0));
    shader.SetMat4("model", model);
    shader.SetVec3("color", color);

    mesh.Draw();
}