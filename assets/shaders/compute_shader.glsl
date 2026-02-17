#version 450 core

#define M_PI 3.1415926535897932384626433832795

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform mat4 cameraToWorld;
uniform vec3 rayOriginWorld;
uniform float fov;
uniform vec3 backgroundColor;

struct Sphere {
    vec3 center;
    float radius;
    vec4 color;
};

bool intersectSphere(vec3 ro, vec3 rd, Sphere s, out float t)
{
    float t0, t1;

    vec3 L = s.center - ro;
    float tca = dot(L, rd);
    if (tca < 0) return false;
    float d2 = dot(L, L) - tca * tca;
    if (d2 > s.radius * s.radius) return false;
    float thc = sqrt(s.radius * s.radius - d2);
    t0 = tca - thc;
    t1 = tca + thc;

    if (t0 > t1)
    {
        float tempT = t1;
        t1 = t0;
        t0 = tempT;
    }

    if (t0 < 0)
    {
        t0 = t1;
        if (t0 < 0) return false;
    }

    t = t0;

    return true;
}

Sphere spheres[] = {
    Sphere(vec3(0.0, 0.0, -5.0), 1.0, vec4(1.0, 1.0, 1.0, 1.0)),
    Sphere(vec3(1.0, 0.0, -4.0), 0.5, vec4(0.4, 0.5, 1.0, 1.0)),
    Sphere(vec3(0.0, 0.0, 5.0), 1.0, vec4(1.0, 0.0, 0.0, 1.0)),
    Sphere(vec3(5.0, 0.0, 0.0), 1.0, vec4(0.0, 1.0, 0.0, 1.0)),
    Sphere(vec3(-5.0, 0.0, 0.0), 1.0, vec4(0.0, 0.0, 1.0, 1.0)),
    Sphere(vec3(0.0, 5.0, 0.0), 1.0, vec4(0.0, 1.0, 1.0, 1.0)),
    Sphere(vec3(0.0, -5.0, 0.0), 1.0, vec4(1.0, 0.0, 1.0, 1.0)),
};

void main()
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    
    ivec2 imgSize = imageSize(imgOutput);
    float aspectRatio = float(imgSize.x) / float(imgSize.y);

    float scale = tan(radians(fov * 0.5));

    // Calculate point (https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays.html)
    float Px = (2.0 * ((float(pixelCoords.x) + 0.5) / float(imgSize.x)) - 1.0) * scale * aspectRatio;
    float Py = (1.0 - 2.0 * ((float(pixelCoords.y) + 0.5) / float(imgSize.y))) * scale;

    // Generate Ray
    vec3 rayPWorld = vec3(cameraToWorld * vec4(Px, Py, -1.0f, 1.0f));
    vec3 rayDir = normalize(rayPWorld - rayOriginWorld);

    // Get nearest intersection
    float nearestT = 1e20;
    vec3 finalColor = backgroundColor;

    for (int i = 0; i < 7; i++)
    {
        float t;
        if (intersectSphere(rayOriginWorld, rayDir, spheres[i], t))
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