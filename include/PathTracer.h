#pragma once

#include <Scene.h>
#include <Camera.h>
#include <ComputeShader.h>
#include <GPUBuffer.h>

class PathTracer
{
private:
    Scene& _scene;
    ComputeShader& _computeShader;
    GPUBuffer _sphereBuffer, _planeBuffer, _pointLightBuffer, _cubeBuffer;

public:
    PathTracer(Scene& scene, ComputeShader& computeShader) : _scene(scene), _computeShader(computeShader) {};

    void PathTrace(const Camera& camera, int width, int height);
};