#include <Scene.h>
#include <Sphere.h>
#include <Image.h>
#include <PathTracer.h>

int main()
{
    Scene scene;
    scene.AddObject(new Sphere(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.25f));
    scene.AddObject(new Sphere(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f));
    scene.AddObject(new Sphere(glm::vec3(1.0f, 0.0f, -8.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f));

    Sphere sphere(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(1.0f), 1.0f);
    Ray ray(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    float t;
    if (sphere.Intersect(ray, t))
    {
        std::cout << "Ray intersects sphere at t = " << t << "\n";
    }
    else
    {
        std::cout << "Ray does not intersect sphere\n";
    }

    Image image(1024, 1024);
    PathTracer::PathTrace(scene, image);
    image.SaveToPPM("output.ppm");
}