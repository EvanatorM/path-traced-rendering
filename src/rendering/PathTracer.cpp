#include <rendering/PathTracer.h>

#include <rendering/Ray.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void PathTracer::PathTrace(const Camera& camera, int width, int height)
{
    // Reset frame count if camera moves
    if (camera.position != _prevCamPos || camera.direction != _prevCamDir)
    {
        _frameCount = 0;
        _prevCamPos = camera.position;
        _prevCamDir = camera.direction;
    }

    // Calculate cameraToWorld matrix
    glm::vec3 target = camera.position + camera.Front();
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 viewMatrix = glm::lookAt(camera.position, target, worldUp);
    glm::mat4 cameraToWorld = glm::inverse(viewMatrix);

    glm::vec3 rayOriginWorld = glm::vec3(cameraToWorld * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    _computeShader.Bind();
    _computeShader.SetMat4("cameraToWorld", cameraToWorld);
    _computeShader.SetVec3("rayOriginWorld", rayOriginWorld);
    _computeShader.SetFloat("fov", camera.fov);
    _computeShader.SetVec3("backgroundColor", camera.backgroundColor);
    _computeShader.SetUInt("frameCount", ++_frameCount);

    // Set scene variables
    if (_sceneDirty)
    {
        auto spheres = _scene.GetGPUSpheres();
        auto planes = _scene.GetGPUPlanes();
        auto pointLights = _scene.GetGPUPointLights();
        auto cubes = _scene.GetGPUCubes();
        auto quadLights = _scene.GetGPUQuadLights();
        auto materials = _scene.GetGPUMaterials();

        _sphereBuffer.BufferData((const void*)spheres.data(), sizeof(GPUSphere) * spheres.size());
        _sphereBuffer.Bind(1);
        _computeShader.SetInt("numSpheres", spheres.size());

        _planeBuffer.BufferData((const void*)planes.data(), sizeof(GPUPlane) * planes.size());
        _planeBuffer.Bind(2);
        _computeShader.SetInt("numPlanes", planes.size());

        _pointLightBuffer.BufferData((const void*)pointLights.data(), sizeof(GPUPointLight) * pointLights.size());
        _pointLightBuffer.Bind(3);
        _computeShader.SetInt("numPointLights", pointLights.size());

        _cubeBuffer.BufferData((const void*)cubes.data(), sizeof(GPUCube) * cubes.size());
        _cubeBuffer.Bind(4);
        _computeShader.SetInt("numCubes", cubes.size());

        _quadLightBuffer.BufferData((const void*)quadLights.data(), sizeof(GPUQuadLight) * quadLights.size());
        _quadLightBuffer.Bind(5);
        _computeShader.SetInt("numQuadLights", quadLights.size());

        _materialBuffer.BufferData((const void*)materials.data(), sizeof(GPUMaterial) * materials.size());
        _materialBuffer.Bind(6);

        _sceneDirty = false;
    }

    _computeShader.Dispatch((unsigned int)std::ceilf(width/16.0f), (unsigned int)std::ceilf(height/16.0f), 1);
    _computeShader.WaitForFinish();
}

void PathTracer::ResetImage()
{
    _frameCount = 0;
}

void PathTracer::SetSceneDirty()
{
    _sceneDirty = true;
}