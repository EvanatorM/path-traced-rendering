#pragma once

#include <Scene.h>
#include <Camera.h>
#include <ComputeShader.h>

class PathTracer
{
private:
    Scene& _scene;
    ComputeShader& _computeShader;
    unsigned int _sphereBuffer, _planeBuffer, _pointLightBuffer;

public:
    PathTracer(Scene& scene, ComputeShader& computeShader);
    ~PathTracer();

    void PathTrace(const Camera& camera, int width, int height);
};