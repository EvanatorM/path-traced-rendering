#pragma once

#include <Scene.h>
#include <Image.h>
#include <Camera.h>

namespace PathTracer
{
    void PathTrace(const Scene& scene, const Camera& camera, Image& outputImage);
}