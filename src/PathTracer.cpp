#include <PathTracer.h>

#include <Ray.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void PathTracer::PathTrace(const Camera& camera, int width, int height)
{
    // Calculate cameraToWorld matrix
    glm::vec3 target = camera.position + camera.Front();
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 viewMatrix = glm::lookAt(camera.position, target, worldUp);
    glm::mat4 cameraToWorld = glm::inverse(viewMatrix);

    glm::vec3 rayOriginWorld = glm::vec3(cameraToWorld * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    auto spheres = _scene.GetGPUSpheres();
    auto planes = _scene.GetGPUPlanes();
    auto pointLights = _scene.GetGPUPointLights();

    _computeShader.Bind();
    _computeShader.SetMat4("cameraToWorld", cameraToWorld);
    _computeShader.SetVec3("rayOriginWorld", rayOriginWorld);
    _computeShader.SetFloat("fov", camera.fov);
    _computeShader.SetVec3("backgroundColor", camera.backgroundColor);

    _sphereBuffer.BufferData((const void*)spheres.data(), sizeof(GPUSphere) * spheres.size());
    _sphereBuffer.Bind(1);
    _computeShader.SetInt("numSpheres", spheres.size());

    _planeBuffer.BufferData((const void*)planes.data(), sizeof(GPUPlane) * planes.size());
    _planeBuffer.Bind(2);
    _computeShader.SetInt("numPlanes", spheres.size());

    _pointLightBuffer.BufferData((const void*)pointLights.data(), sizeof(GPUPointLight) * pointLights.size());
    _pointLightBuffer.Bind(3);
    _computeShader.SetInt("numPointLights", spheres.size());

    glDispatchCompute((unsigned int)std::ceilf(width/16.0f), (unsigned int)std::ceilf(height/16.0f), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}