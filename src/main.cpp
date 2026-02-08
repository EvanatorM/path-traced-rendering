#include <Scene.h>
#include <Sphere.h>
#include <Plane.h>
#include <Image.h>
#include <PathTracer.h>
#include <Renderer.h>
#include <Window.h>

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

    // Create window
    Renderer::Init();
    Window::InitWindow(640, 400, "Path Traced Renderer");
    auto& window = Window::GetInstance();
    
    while (!window.ShouldClose())
    {
        // Clear window
        window.Clear();

        // End-of-frame logic
        window.SwapBuffers();
        window.PollEvents();
    }

    Renderer::Shutdown();
}