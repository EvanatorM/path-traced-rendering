#pragma once

#include <scenes/Scene.h>
#include <rendering/Ray.h>
#include <glm/glm.hpp>

namespace Raycast
{
    struct RaycastResult
    {
        float dist;
        SceneObject* objectHit;
    };

    bool Cast(Scene& scene, const Ray& ray, float maxDistance, RaycastResult& result)
    {
        bool hit = false;
        float minDist = 100000000.0f;
        SceneObject* minObj = nullptr;

        auto objects = scene.GetSceneObjects();

        for (auto* obj : objects)
        {
            float d;
            if (obj->OverlapRay(ray, d))
            {
                if (d < minDist && d < maxDistance)
                {
                    hit = true;
                    minDist = d;
                    minObj = obj;
                }
            }
        }

        result = { minDist, minObj };
        return hit;
    }
}