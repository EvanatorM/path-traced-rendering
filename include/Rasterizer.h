#pragma once

#include <Scene.h>
#include <Camera.h>
#include <Shader.h>
#include <GPUBuffer.h>

class Rasterizer
{
private:
    Scene& _scene;
    GPUBuffer _pointLightBuffer;
    Shader& _shader;

public:
    Rasterizer(Scene& scene, Shader& shader) : _scene(scene), _shader(shader) {};

    void Render(const Camera& camera);
};