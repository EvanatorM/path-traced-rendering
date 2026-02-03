#include <PathTracer.h>

#include <Ray.h>

namespace PathTracer
{
    void PathTrace(const Scene& scene, Image& image)
    {
        auto objects = scene.GetObjects();
        float sx = 3.0f / image.width;
        float sy = 3.0f / image.height;
        for (int x = 0; x < image.width; x++)
        {
            for (int y = 0; y < image.height; y++)
            {
                // Generate ray from camera
                Ray ray(glm::vec3(x * sx - 1.5f, y * sy - 1.5f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));

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
                    image.SetPixel(x, y, 
                        static_cast<unsigned char>(hitObject->color.r * 255),
                        static_cast<unsigned char>(hitObject->color.g * 255),
                        static_cast<unsigned char>(hitObject->color.b * 255));
                }
                else
                {
                    image.SetPixel(x, y, 0, 0, 0); // Background color
                }
            }
        }
    }
}