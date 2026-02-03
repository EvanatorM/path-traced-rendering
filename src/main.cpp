#include <Scene.h>
#include <Sphere.h>
#include <Image.h>
#include <PathTracer.h>

int main()
{
    Scene scene;
    scene.AddObject(Sphere());

    Image image(64, 64);
    PathTracer::PathTrace(scene, image);
    image.SaveToPPM("output.ppm");
}