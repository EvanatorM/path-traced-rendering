#version 450 core

#define M_PI 3.1415926535897932384626433832795

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform mat4 cameraToWorld;
uniform vec3 rayOriginWorld;
uniform float fov;

struct Sphere {
    vec3 center;
    float radius;
    vec4 color;
};

bool intersectSphere(vec3 ro, vec3 rd, Sphere s, out float t)
{
    vec3 oc = ro - s.center;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - s.radius * s.radius;
    float h = b * b - c;
    
    if (h < 0.0) return false; // No intersection
    
    float sqrtH = sqrt(h);
    float t0 = -b - sqrtH; // First hit
    float t1 = -b + sqrtH; // Second hit
    
    if (t0 > 0.001) { // Check if the closest hit is in front of the ray
        t = t0;
        return true;
    }
    if (t1 > 0.001) { // Check if we are inside the sphere
        t = t1;
        return true;
    }
    
    return false;
}

Sphere spheres[] = {
    Sphere(vec3(0.0, 0.0, -5.0), 1.0, vec4(1.0, 1.0, 1.0, 1.0)),
    Sphere(vec3(1.0, 0.0, -4.0), 0.5, vec4(0.4, 0.5, 1.0, 1.0)),
};

void main()
{
    ivec2 size = imageSize(imgOutput);
    float aspectRatio = float(size.x) / float(size.y);
    float scale = tan(radians(fov * 0.5));

    // Calculate point (https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays.html)
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    float Px = (2.0 * ((float(pixelCoords.x) + 0.5) / float(size.x)) - 1.0) * tan(fov / 2.0 * M_PI / 180.0) * aspectRatio;
    float Py = (1.0 - 2.0 * ((float(pixelCoords.y) + 0.5) / float(size.y))) * tan(fov / 2.0 * M_PI / 180.0);

    // Ray Generation
    //vec3 rayDir = normalize(vec3(cameraToWorld * vec4(Px, Py, -1.0, 0.0)));
    vec3 rayDir = normalize(vec3(Px, Py, -1.0));
    
    // The Intersection Loop
    float nearestT = 1e20;
    vec3 finalColor = vec3(0.0); // Background

    for (int i = 0; i < 2; i++)
    {
        float t;
        if (intersectSphere(vec3(0.0), rayDir, spheres[i], t))
        {
            if (t < nearestT)
            {
                nearestT = t;
                finalColor = spheres[i].color.rgb;
            }
        }
    }

    imageStore(imgOutput, pixelCoords, vec4(finalColor, 1.0));
}