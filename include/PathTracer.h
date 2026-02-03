#pragma once

#include <Scene.h>
#include <Image.h>

namespace PathTracer
{
    void PathTrace(const Scene& scene, Image& image);
}