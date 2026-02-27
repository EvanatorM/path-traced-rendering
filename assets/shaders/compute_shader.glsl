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

struct Plane {
    vec3 offset;
    float padding1;
    vec3 orientation;
    float padding2;
    vec4 color;
};

struct PointLight {
    vec3 position;
    float attenuation;
    vec3 color;
    float intensity;
};

struct Cube {
    vec4 position;
    vec4 size;
    vec4 color;
};

layout(std430, binding = 1) buffer sphereBuffer
{
    Sphere[] spheres;
};
uniform int numSpheres;

layout(std430, binding = 2) buffer planeBuffer
{
    Plane[] planes;
};
uniform int numPlanes;

layout(std430, binding = 3) buffer pointLightBuffer
{
    PointLight[] pointLights;
};
uniform int numPointLights;

layout(std430, binding = 4) buffer cubeBuffer
{
    Cube[] cubes;
};
uniform int numCubes;

// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
bool intersectSphere(vec3 ro, vec3 rd, Sphere s, out float t, out vec3 hitPoint, out vec3 normal)
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
    hitPoint = ro + rd * t;
    normal = normalize(hitPoint - s.center);

    return true;
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html
bool intersectPlane(vec3 ro, vec3 rd, Plane p, out float t, out vec3 hitPoint, out vec3 normal)
{
    float denom = dot(p.orientation, rd);
    if (denom > 1e-6)
    {
        vec3 p010 = p.offset - ro;
        t = dot(p010, p.orientation) / denom;

        if (t < 0) return false;

        hitPoint = ro + rd * t;
        normal = (denom < 0.0) ? normalize(p.orientation) : -normalize(p.orientation);
        return true;
    }

    return false;
}

// https://people.csail.mit.edu/amy/papers/box-jgt.pdf
bool intersectCube(vec3 ro, vec3 rd, Cube c, out float t, out vec3 hitPoint, out vec3 normal)
{
    vec3 halfSize = c.size.xyz * 0.5;
    vec3 bmin = c.position.xyz - halfSize;
    vec3 bmax = c.position.xyz + halfSize;

    vec3 t0 = (bmin - ro) / rd;
    vec3 t1 = (bmax - ro) / rd;

    vec3 tsmaller = min(t0, t1);
    vec3 tbigger  = max(t0, t1);

    float tNear = max(max(tsmaller.x, tsmaller.y), tsmaller.z);
    float tFar  = min(min(tbigger.x, tbigger.y), tbigger.z);

    if (tNear > tFar || tFar < 0.0)
        return false;

    t = (tNear > 0.0) ? tNear : tFar;
    if (t < 0.0)
        return false;

    hitPoint = ro + rd * t;

    vec3 local = hitPoint - c.position.xyz;
    vec3 faceDist = abs(halfSize - abs(local));

    if (faceDist.x < faceDist.y && faceDist.x < faceDist.z)
        normal = vec3(sign(local.x), 0.0, 0.0);
    else if (faceDist.y < faceDist.z)
        normal = vec3(0.0, sign(local.y), 0.0);
    else
        normal = vec3(0.0, 0.0, sign(local.z));

    return true;
}

bool rayBlocked(vec3 ro, vec3 rd, float maxT)
{
    float t;
    vec3 hitPoint;
    vec3 hitNormal;
    for (int i = 0; i < numSpheres; i++)
        if (intersectSphere(ro, rd, spheres[i], t, hitPoint, hitNormal) && t > 1e-4 && t < maxT) return true;

    for (int i = 0; i < numPlanes; i++)
        if (intersectPlane(ro, rd, planes[i], t, hitPoint, hitNormal) && t > 1e-4 && t < maxT) return true;

    return false;
}

// https://deepwiki.com/nico-mayora/gpu_data_structures/3.2-direct-and-indirect-illumination
vec3 calculateDirectLighting(vec3 hitPoint, vec3 hitPointNormal)
{
    vec3 lighting = vec3(0.0);

    for (int i = 0; i < numPointLights; i++)
    {
        // Calculate light direction
        vec3 toLight = pointLights[i].position - hitPoint;
        float dist2 = max(dot(toLight, toLight), 1e-6);
        vec3 L = toLight * inversesqrt(dist2);

        float NdotL = max(dot(hitPointNormal, L), 0.0);
        if (NdotL <= 0.0)
            continue;

        vec3 shadowOrigin = hitPoint + hitPointNormal * 1e-3;
        if (rayBlocked(hitPoint, L, sqrt(dist2)))
            continue;

        vec3 radiance = pointLights[i].color * pointLights[i].intensity / (dist2 / pointLights[i].attenuation);
        lighting += radiance * NdotL;
    }

    return lighting;
}

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
    int nearestObjType = -1;
    int nearestIndex = 0;
    vec3 nearestHitPoint;
    vec3 nearestHitNormal;
    vec3 finalColor = backgroundColor;

    for (int i = 0; i < numSpheres; i++)
    {
        float t;
        vec3 hitPoint;
        vec3 hitNormal;
        if (intersectSphere(rayOriginWorld, rayDir, spheres[i], t, hitPoint, hitNormal))
        {
            if (t < nearestT)
            {
                nearestT = t;
                nearestObjType = 0;
                nearestIndex = i;
                nearestHitPoint = hitPoint;
                nearestHitNormal = hitNormal;
            }
        }
    }

    for (int i = 0; i < numPlanes; i++)
    {
        float t;
        vec3 hitPoint;
        vec3 hitNormal;
        if (intersectPlane(rayOriginWorld, rayDir, planes[i], t, hitPoint, hitNormal))
        {
            if (t < nearestT)
            {
                nearestT = t;
                nearestObjType = 1;
                nearestIndex = i;
                nearestHitPoint = hitPoint;
                nearestHitNormal = hitNormal;
            }
        }
    }

    for (int i = 0; i < numCubes; i++)
    {
        float t;
        vec3 hitPoint;
        vec3 hitNormal;
        if (intersectCube(rayOriginWorld, rayDir, cubes[i], t, hitPoint, hitNormal))
        {
            if (t < nearestT)
            {
                nearestT = t;
                nearestObjType = 2;
                nearestIndex = i;
                nearestHitPoint = hitPoint;
                nearestHitNormal = hitNormal;
            }
        }
    }

    switch (nearestObjType)
    {
        case 0: // Spheres
            vec3 sLighting = calculateDirectLighting(nearestHitPoint, nearestHitNormal);
            finalColor = spheres[nearestIndex].color.rgb * sLighting;
            break;
        case 1: // Planes
            vec3 pLighting = calculateDirectLighting(nearestHitPoint, nearestHitNormal);
            finalColor = planes[nearestIndex].color.rgb * pLighting;
            break;
        case 2: // Cubes
            vec3 cLighting = calculateDirectLighting(nearestHitPoint, nearestHitNormal);
            finalColor = cubes[nearestIndex].color.rgb * cLighting;
            break;
    }

    imageStore(imgOutput, pixelCoords, vec4(finalColor, 1.0));
}