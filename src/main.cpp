#include <Scene.h>
#include <Sphere.h>
#include <Plane.h>
#include <Image.h>
#include <PathTracer.h>

int main()
{
    Scene scene;
    scene.AddObject(new Plane(glm::vec3(0.0f, -3.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f)));
    scene.AddObject(new Sphere(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.25f));
    scene.AddObject(new Sphere(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f));
    scene.AddObject(new Sphere(glm::vec3(1.0f, 0.0f, -8.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f));
    scene.AddObject(new Sphere(glm::vec3(-1.0f, -1.0f, -8.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.5f));

    Camera camera;

    Image image(1280, 720);
    PathTracer::PathTrace(scene, camera, image);
    image.SaveToPPM("output.ppm");
}