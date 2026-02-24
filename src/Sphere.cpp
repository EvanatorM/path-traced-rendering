#include <Sphere.h>

#include <Mesh.h>
#include <Shader.h>
#include <glm/gtc/matrix_transform.hpp>

void Sphere::RenderRaster(const glm::mat4& view, const glm::mat4& proj) const
{
    static Mesh mesh = Mesh::CreateFromFile("assets/models/sphere.obj");
    static Shader shader("assets/shaders/shape.vert", "assets/shaders/shape.frag");

    shader.Bind();
    shader.SetMat4("view", view);
    shader.SetMat4("proj", proj);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(radius * 2.0f));
    shader.SetMat4("model", model);
    shader.SetVec3("color", color);

    mesh.Draw();
}