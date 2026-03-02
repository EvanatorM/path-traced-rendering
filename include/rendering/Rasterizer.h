#pragma once

#include <scenes/Scene.h>
#include <rendering/Camera.h>
#include <rendering/render-objects/Shader.h>
#include <rendering/render-objects/GPUBuffer.h>

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