#pragma once

#include <Sphere.h>
#include <Plane.h>
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
    
    const std::vector<Sphere>& GetSpheres() const { return _spheres; }
    const std::vector<Plane>& GetPlanes() const { return _planes; }
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

private:
    std::vector<Sphere> _spheres;
    std::vector<Plane> _planes;
};