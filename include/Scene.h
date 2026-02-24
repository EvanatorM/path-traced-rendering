#pragma once

#include <Sphere.h>
#include <Plane.h>
#include <PointLight.h>
#include <vector>

class Scene
{
public:
    void AddSphere(Sphere sphere)
    {
        _spheres.push_back(std::move(sphere));
    }
    void AddPlane(Plane plane)
    {
        _planes.push_back(std::move(plane));
    }
    void AddPointLight(PointLight pointLight)
    {
        _pointLights.push_back(std::move(pointLight));
    }
    
    const std::vector<Sphere>& GetSpheres() const { return _spheres; }
    const std::vector<Plane>& GetPlanes() const { return _planes; }
    const std::vector<PointLight>& GetPointLights() const { return _pointLights; }
    std::vector<GPUSphere> GetGPUSpheres() const
    { 
        std::vector<GPUSphere> spheres;
        for (auto& sphere : _spheres)
            spheres.push_back(sphere.GetGPUSphere());
        return spheres; 
    }
    std::vector<GPUPlane> GetGPUPlanes() const 
    { 
        std::vector<GPUPlane> planes;
        for (auto& plane : _planes)
            planes.push_back(plane.GetGPUPlane());
        return planes; 
    }
    std::vector<GPUPointLight> GetGPUPointLights() const
    {
        std::vector<GPUPointLight> pointLights;
        for (auto& pointLight : _pointLights)
            pointLights.push_back(pointLight.GetGPUPointLight());
        return pointLights;
    }
private:
    std::vector<Sphere> _spheres;
    std::vector<Plane> _planes;
    std::vector<PointLight> _pointLights;
};