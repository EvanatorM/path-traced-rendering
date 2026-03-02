#pragma once

#include <scenes/Scene.h>
#include <rendering/Camera.h>
#include <rendering/render-objects/ComputeShader.h>
#include <rendering/render-objects/GPUBuffer.h>

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