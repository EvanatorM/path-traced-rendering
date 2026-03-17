#include <rendering/Rasterizer.h>

void Rasterizer::Render(const Camera& camera)
{
    auto pointLights = _scene.GetGPUPointLights();
    _pointLightBuffer.BufferData((const void*)pointLights.data(), sizeof(GPUPointLight) * pointLights.size());
    _pointLightBuffer.Bind(3);

    auto quadLights = _scene.GetGPUQuadLights();
    _quadLightBuffer.BufferData((const void*)quadLights.data(), sizeof(GPUQuadLight) * quadLights.size());
    _quadLightBuffer.Bind(5);

    auto view = camera.GetViewMatrix();
    auto proj = camera.GetProjectionMatrix();

    _shader.Bind();
    _shader.SetMat4("view", view);
    _shader.SetMat4("proj", proj);
    _shader.SetVec3("viewPos", camera.position);

    _shader.SetInt("numPointLights", pointLights.size());
    _shader.SetInt("numQuadLights", quadLights.size());

    for (auto& plane : _scene.GetPlanes())
    {
        plane.RenderRaster(_shader);
    }
    for (auto& sphere : _scene.GetSpheres())
    {
        sphere.RenderRaster(_shader);
    }
    for (auto& cube : _scene.GetCubes())
    {
        cube.RenderRaster(_shader);
    }
}