#include <scenes/Sphere.h>

#include <rendering/render-objects/Mesh.h>
#include <glm/gtc/matrix_transform.hpp>

void Sphere::RenderRaster(Shader& shader) const
{
    static Mesh mesh = Mesh::CreateFromFile("assets/models/sphere.obj");

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(radius * 2.0f));
    shader.SetMat4("model", model);

    mesh.Draw();
}

bool Sphere::OverlapRay(const Ray& ray, float& dist) const
{
    glm::vec3 L = position - ray.origin;
    float tca = glm::dot(L, ray.direction);
    if (tca < 0) return false;
    float d2 = glm::dot(L, L) - tca * tca;
    if (d2 > radius * radius) return false;
    float thc = glm::sqrt(radius * radius - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;

    if (t0 > t1)
    {
        float tempT = t1;
        t1 = t0;
        t0 = tempT;
    }

    if (t0 < 0)
    {
        t0 = t1;
        if (t0 < 0) return false;
    }

    dist = t0;

    return true;
}