#pragma once

#include <SceneObject.h>
#include <vector>

class Scene
{
public:
    void AddObject(SceneObject&& object)
    {
        _objects.push_back(std::move(object));
    }
    void AddObject(const SceneObject& object)
    {
        _objects.push_back(object);
    }

private:
    std::vector<SceneObject> _objects;
};