#pragma once

#include <Sphere.h>
#include <Plane.h>
#include <PointLight.h>
#include <Cube.h>
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
    void AddCube(Cube cube)
    {
        _cubes.push_back(std::move(cube));
    }
    
    const std::vector<Sphere>& GetSpheres() const { return _spheres; }
    const std::vector<Plane>& GetPlanes() const { return _planes; }
    const std::vector<PointLight>& GetPointLights() const { return _pointLights; }
    const std::vector<Cube>& GetCubes() const { return _cubes; }
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
    std::vector<GPUCube> GetGPUCubes() const
    {
        std::vector<GPUCube> cubes;
        for (auto& cube : _cubes)
            cubes.push_back(cube.GetGPUCube());
        return cubes;
    }
private:
    std::vector<Sphere> _spheres;
    std::vector<Plane> _planes;
    std::vector<PointLight> _pointLights;
    std::vector<Cube> _cubes;
};