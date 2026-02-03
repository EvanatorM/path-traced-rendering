#pragma once

#include <SceneObject.h>
#include <vector>

class Scene
{
public:
    void AddObject(SceneObject* object)
    {
        _objects.push_back(object);
    }
    
    const std::vector<SceneObject*>& GetObjects() const { return _objects; }

private:
    std::vector<SceneObject*> _objects;
};