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

    mesh.Draw();
}

bool Cube::OverlapRay(const Ray& ray, float& dist) const
{
    glm::vec3 halfSize = size * 0.5f;
    glm::vec3 bmin = position - halfSize;
    glm::vec3 bmax = position + halfSize;

    glm::vec3 t0 = (bmin - ray.origin) / ray.direction;
    glm::vec3 t1 = (bmax - ray.origin) / ray.direction;

    glm::vec3 tsmaller = glm::min(t0, t1);
    glm::vec3 tbigger  = glm::max(t0, t1);

    float tNear = glm::max(glm::max(tsmaller.x, tsmaller.y), tsmaller.z);
    float tFar  = glm::min(glm::min(tbigger.x, tbigger.y), tbigger.z);

    if (tNear > tFar || tFar < 0.0f)
        return false;

    dist = (tNear >= 0.0f) ? tNear : tFar;
    if (dist < 0.0f)
        return false;

    return true;
}