#include <PathTracer.h>

#include <Ray.h>
#include <glm/gtc/matrix_transform.hpp>

#define PI 3.14159265358979323846

namespace PathTracer
{
    void PathTrace(const Scene& scene, const Camera& camera, Image& outputImage)
    {
        // Calculate cameraToWorld matrix
        glm::vec3 target = camera.position + camera.direction;
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 viewMatrix = glm::lookAt(camera.position, target, worldUp);
        glm::mat4 cameraToWorld = glm::inverse(viewMatrix);

        glm::vec3 rayOriginWorld = glm::vec3(cameraToWorld * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        auto objects = scene.GetObjects();
        for (int x = 0; x < outputImage.width; x++)
        {
            for (int y = 0; y < outputImage.height; y++)
            {
                // Calculate point (https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays.html)
                float Px = (2 * ((x + 0.5f) / outputImage.width) - 1) * glm::tan(camera.fov / 2 * PI / 180) * outputImage.aspectRatio;
                float Py = (1 - 2 * ((y + 0.5f) / outputImage.height)) * glm::tan(camera.fov / 2 * PI / 180);

                // Generate ray
                glm::vec3 rayPWorld = glm::vec3(cameraToWorld * glm::vec4(Px, Py, -1.0f, 1.0f));
                Ray ray(rayOriginWorld, glm::normalize(rayPWorld - rayOriginWorld));

                // Get nearest intersection
                float nearestT = std::numeric_limits<float>::max();
                const SceneObject* hitObject = nullptr;
                for (const auto& obj : objects)
                {
                    float t;
                    if (obj->Intersect(ray, t))
                    {
                        if (t < nearestT)
                        {
                            nearestT = t;
                            hitObject = obj;
                        }
                    }
                }

                // Get pixel color
                if (hitObject)
                {
                    outputImage.SetPixel(x, y, 
                        static_cast<unsigned char>(hitObject->color.r * 255),
                        static_cast<unsigned char>(hitObject->color.g * 255),
                        static_cast<unsigned char>(hitObject->color.b * 255));
                }
                else
                {
                    outputImage.SetPixel(x, y, 
                        static_cast<unsigned char>(camera.backgroundColor.r * 255),
                        static_cast<unsigned char>(camera.backgroundColor.g * 255),
                        static_cast<unsigned char>(camera.backgroundColor.b * 255));
                }
            }
        }
    }
}