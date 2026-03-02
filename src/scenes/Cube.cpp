#include <scenes/Cube.h>

#include <rendering/render-objects/Mesh.h>
#include <glm/gtc/matrix_transform.hpp>

void Cube::RenderRaster(Shader& shader) const
{
    static Mesh mesh = Mesh::CreateFromFile("assets/models/cube.obj");

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, size);
    shader.SetMat4("model", model);
    shader.SetVec3("color", color);

    mesh.Draw();
}