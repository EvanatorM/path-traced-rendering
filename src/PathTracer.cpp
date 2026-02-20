#include <PathTracer.h>

#include <Ray.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

PathTracer::PathTracer(Scene& scene, ComputeShader& computeShader)
    : _scene(scene), _computeShader(computeShader)
{
    glCreateBuffers(1, &_sphereBuffer);
    glCreateBuffers(1, &_planeBuffer);
}
PathTracer::~PathTracer()
{
    glDeleteBuffers(1, &_sphereBuffer);
    glDeleteBuffers(1, &_planeBuffer);
}

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

    _computeShader.Bind();
    _computeShader.SetMat4("cameraToWorld", cameraToWorld);
    _computeShader.SetVec3("rayOriginWorld", rayOriginWorld);
    _computeShader.SetFloat("fov", camera.fov);
    _computeShader.SetVec3("backgroundColor", camera.backgroundColor);

    
    glNamedBufferStorage(_sphereBuffer, sizeof(GPUSphere) * spheres.size(), (const void*)spheres.data(), GL_DYNAMIC_STORAGE_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _sphereBuffer);
    _computeShader.SetInt("numSpheres", spheres.size());

    glNamedBufferStorage(_planeBuffer, sizeof(GPUPlane) * planes.size(), (const void*)planes.data(), GL_DYNAMIC_STORAGE_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _planeBuffer);
    _computeShader.SetInt("numPlanes", planes.size());

    glDispatchCompute((unsigned int)width/16, (unsigned int)height/16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}