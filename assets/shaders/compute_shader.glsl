#version 450 core

#define M_PI 3.1415926535897932384626433832795
#define MAX_BOUNCES 4
#define BIAS 0.001

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

// ----------------------------------------
//               Scene Info
// ----------------------------------------

uniform mat4 cameraToWorld;
uniform vec3 rayOriginWorld;
uniform float fov;
uniform vec3 backgroundColor;
uniform uint frameCount;

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

struct Cube {
    vec4 position;
    vec4 size;
    vec4 color;
};

struct PointLight {
    vec3 position;
    float attenuation;
    vec3 color;
    float intensity;
};

struct QuadLight {
    vec3 position;
    float intensity;
    vec3 u;
    float attenuation;
    vec3 v;
    float padding1;
    vec3 color;
    float padding2;
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

layout(std430, binding = 5) buffer quadLightBuffer
{
    QuadLight[] quadLights;
};
uniform int numQuadLights;

// ----------------------------------------
//                   RNG
// ----------------------------------------

// PCG32 PRNG implementation based on
// "PCG random number generators in glsl" by Riccardo https://observablehq.com/@riccardoscalco/pcg-random-number-generators-in-glsl
uint pcg_state;

uint pcg_hash()
{
    uint state = pcg_state;
    pcg_state = pcg_state * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float randomFloat()
{
    return float(pcg_hash()) * (1.0 / 4294967295.0);
}

// ----------------------------------------
//               Intersection
// ----------------------------------------

// Using a modified version of the function created by
// "A Minimal Ray-Tracer: Ray-Sphere Intersection" by Jean-Colas Prunier https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
bool intersectSphere(vec3 ro, vec3 rd, Sphere s, out float t, out vec3 normal)
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
    vec3 hitPoint = ro + rd * t;
    normal = normalize(hitPoint - s.center);

    return true;
}

// Using a modified version of the function created by
// "A Minimal Ray-Tracer: Ray-Plane and Ray-Disk Intersection" by Jean-Colas Prunier https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html
bool intersectPlane(vec3 ro, vec3 rd, Plane p, out float t, out vec3 normal)
{
    float denom = dot(p.orientation, rd);
    if (denom > 1e-6)
    {
        vec3 p010 = p.offset - ro;
        t = dot(p010, p.orientation) / denom;

        if (t < 0) return false;

        vec3 hitPoint = ro + rd * t;
        normal = (denom < 0.0) ? normalize(p.orientation) : -normalize(p.orientation);
        return true;
    }

    return false;
}

// Using a modified version based on the contents of
// "An Efficient and Robust Ray–Box Intersection Algorithm" by Amy Williams et al. https://people.csail.mit.edu/amy/papers/box-jgt.pdf
bool intersectCube(vec3 ro, vec3 rd, Cube c, out float t, out vec3 normal)
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

    vec3 hitPoint = ro + rd * t;

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

bool intersectQuad(vec3 ro, vec3 rd, QuadLight q, out float t, out vec3 normal)
{
    normal = normalize(cross(q.u, q.v));

    float denom = dot(normal, rd);
    if (abs(denom) < 1e-6) return false;

    vec3 p0l0 = q.position - ro;
    t = dot(p0l0, normal) / denom;
    if (t < 0.0) return false;

    vec3 hitPoint = ro + rd * t;
    vec3 vi = hitPoint - q.position;

    float uu = dot(vi, q.u) / dot(q.u, q.u);
    float vv = dot(vi, q.v) / dot(q.v, q.v);

    return (uu >= 0.0 && uu <= 1.0 && vv >= 0.0 && vv <= 1.0);
}

bool getSceneIntersection(vec3 ro, vec3 rd, out float closestT, out vec3 normal, out vec3 albedo, out vec3 emission, out bool hitLight)
{
    closestT = 1e20;
    bool hit = false;
    hitLight = false;
    emission = vec3(0.0);
    float t;
    vec3 n;

    for (int i = 0; i < numSpheres; i++)
    {
        if (intersectSphere(ro, rd, spheres[i], t, n) && t < closestT)
        {
            closestT = t;
            normal = n;
            albedo = spheres[i].color.rgb;
            hit = true;
        }
    }
    for(int i = 0; i < numPlanes; i++)
    {
        if(intersectPlane(ro, rd, planes[i], t, n) && t < closestT)
        {
            closestT = t;
            normal = n;
            albedo = planes[i].color.rgb;
            hit = true;
        }
    }
    for(int i = 0; i < numCubes; i++)
    {
        if(intersectCube(ro, rd, cubes[i], t, n) && t < closestT)
        {
            closestT = t;
            normal = n;
            albedo = cubes[i].color.rgb;
            hit = true;
        }
    }
    for (int i = 0; i < numQuadLights; i++)
    {
        if (intersectQuad(ro, rd, quadLights[i], t, n) && t < closestT)
        {
            closestT = t;
            normal = n;
            hitLight = true;
            emission = quadLights[i].color * quadLights[i].intensity;
            hit = true;
        }
    }

    return hit;
}

bool rayBlocked(vec3 ro, vec3 rd, float maxT)
{
    float t;
    vec3 hitNormal;
    for (int i = 0; i < numSpheres; i++)
        if (intersectSphere(ro, rd, spheres[i], t, hitNormal) && t > 1e-4 && t < maxT) return true;

    for (int i = 0; i < numPlanes; i++)
        if (intersectPlane(ro, rd, planes[i], t, hitNormal) && t > 1e-4 && t < maxT) return true;

    for (int i = 0; i < numCubes; i++)
        if (intersectCube(ro, rd, cubes[i], t, hitNormal) && t > 1e-4 && t < maxT) return true;

    return false;
}

// ----------------------------------------
//                Sampling
// ----------------------------------------

// Duff et al. 2017 Orthonormal Basis
void buildOrthonormalBasis(vec3 N, out vec3 T, out vec3 B)
{
    float sign = N.z >= 0.0 ? 1.0 : - 1.0;
    float a = - 1.0 / (sign + N.z);
    float b = N.x * N.y * a;
    T = vec3(1.0 + sign * N.x * N.x * a, sign * b, - sign * N.x);
    B = vec3(b, sign + N.y * N.y * a, - N.y);
}

// Malley's Method for Cosine-Weighted Hemisphere Sampling
vec3 cosineSampleHemisphere(float u1, float u2)
{
    float r = sqrt(u1);
    float theta = 2.0 * M_PI * u2;
    float x = r * cos(theta);
    float y = r * sin(theta);
    float z = sqrt(max(0.0, 1.0 - x * x - y * y));
    return vec3(x, y, z);
}

// ----------------------------------------
//              Path Tracing
// ----------------------------------------

vec3 tracePath(vec3 ro, vec3 rd)
{
    vec3 radiance = vec3(0.0);
    vec3 throughput = vec3(1.0);
    vec3 currentRayOrigin = ro;
    vec3 currentRayDir = rd;

    for (int bounce = 0; bounce < MAX_BOUNCES; bounce++)
    {
        float t;
        vec3 normal;
        vec3 albedo;
        vec3 emission;
        bool hitLight;

        if (!getSceneIntersection(currentRayOrigin, currentRayDir, t, normal, albedo, emission, hitLight))
        {
            radiance += throughput * backgroundColor;
            break;
        }

        // Handle hitting a light source directly
        if (hitLight)
        {
            // Only add emission if it's the first camera ray
            // When adding specular/mirror materials, emission will be added if the prev bounce is perfectly specular
            if (bounce == 0)
                radiance += throughput * emission;

            // Stop tracing when light is hit
            break;
        }

        vec3 hitPoint = currentRayOrigin + currentRayDir * t;

        // 1. Direct Illumination
        vec3 directLighting = vec3(0.0);

        // Point Lights
        for (int i = 0; i < numPointLights; i++)
        {
            vec3 toLight = pointLights[i].position - hitPoint;
            float dist2 = dot(toLight, toLight);
            float dist = sqrt(dist2);
            vec3 L = toLight / dist;

            float NdotL = max(dot(normal, L), 0.0);
            if (NdotL > 0.0)
            {
                // Cast shadow ray
                if (!rayBlocked(hitPoint + normal * BIAS, L, dist))
                {
                    float attenRad = pointLights[i].attenuation;
                    float attenuation = 1.0 / (dist2 + attenRad * attenRad);
                    vec3 lightIntensity = pointLights[i].color * pointLights[i].intensity * attenuation;
                    // BRDF is albedo / PI
                    directLighting += lightIntensity * NdotL * (albedo / M_PI);
                }
            }
        }
        // Quad Lights
        for (int i = 0; i < numQuadLights; i++)
        {
            // Sample a random point on the light
            float u1 = randomFloat();
            float u2 = randomFloat();
            vec3 lightPoint = quadLights[i].position + (quadLights[i].u * u1) + (quadLights[i].v * u2);

            // Calculate distance and direction to light
            vec3 toLight = lightPoint - hitPoint;
            float dist2 = dot(toLight, toLight);
            float dist = sqrt(dist2);
            vec3 L = toLight / dist;

            // Check if light is hitting the front of the surface
            float NdotL = max(dot(normal, L), 0.0);
            if (NdotL > 0.0)
            {
                // Calculate the normal of the light
                vec3 lightNormal = normalize(cross(quadLights[i].u, quadLights[i].v));
                // Check if the surface is facing the front of the light
                float lightCos = max(dot(lightNormal, -L), 0.0);

                if (lightCos > 0.0)
                {
                    // Cast shadow ray
                    if (!rayBlocked(hitPoint + normal * BIAS, L, dist))
                    {
                        float attenRad = quadLights[i].attenuation;
                        float attenuation = 1.0 / (dist2 + attenRad * attenRad);
                        vec3 lightIntensity = quadLights[i].color * quadLights[i].intensity * attenuation;
                        // BRDF is albedo / PI
                        directLighting += lightIntensity * NdotL * (albedo / M_PI);
                    }
                }
            }
        }
        radiance += throughput * directLighting;

        // 2. Indirect Lighting (Monte Carlo)
        // Based on methods found in
        // "Global Illumination and Path Tracing: a Practical Implementation" by Jean-Colas Prunier https://www.scratchapixel.com/lessons/3d-basic-rendering/global-illumination-path-tracing/global-illumination-path-tracing-practical-implementation.html
        float u1 = randomFloat();
        float u2 = randomFloat();
        vec3 localSample = cosineSampleHemisphere(u1, u2);

        vec3 T, B;
        buildOrthonormalBasis(normal, T, B);

        // Transform local sample to world space
        vec3 worldSample = localSample.x * T + localSample.y * B + localSample.z * normal;

        // Setup the ray for the next iteration
        currentRayDir = worldSample;
        currentRayOrigin = hitPoint + normal * BIAS;

        throughput *= albedo;

        // Russian Roulette termination
        float maxThroughput = max(max(throughput.x, throughput.y), throughput.z);
        if (maxThroughput < 0.001) break;
    }

    return radiance;
}

// ----------------------------------------
//                   Main
// ----------------------------------------

void main()
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    
    ivec2 imgSize = imageSize(imgOutput);
    float aspectRatio = float(imgSize.x) / float(imgSize.y);

    float scale = tan(radians(fov * 0.5));

    // Seed PRNG
    uint linearIndex = pixelCoords.y * imgSize.x + pixelCoords.x;
    pcg_state = linearIndex + frameCount * 719393u;

    // Ray generation algorithm below is based on 
    // "Generating Camera Rays with Ray-Tracing: Generating Camera Rays" by Jean-Colas Prunier https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays.html
    
    // Add jitter for anti-aliasing
    float jitterX = randomFloat();
    float jitterY = randomFloat();
    // Calculate point
    float Px = (2.0 * ((float(pixelCoords.x) + jitterX) / float(imgSize.x)) - 1.0) * scale * aspectRatio;
    float Py = (1.0 - 2.0 * ((float(pixelCoords.y) + jitterY) / float(imgSize.y))) * scale;

    // Generate Ray
    vec3 rayPWorld = vec3(cameraToWorld * vec4(Px, Py, -1.0f, 1.0f));
    vec3 rayDir = normalize(rayPWorld - rayOriginWorld);

    // Calculate color
    vec3 finalColor = tracePath(rayOriginWorld, rayDir);

    // Add color to image
    if (frameCount == 1) // New image, no averaging
        imageStore(imgOutput, pixelCoords, vec4(finalColor, 1.0));
    else
    {
        // Average the new color and existing color for progressive rendering
        vec3 oldColor = imageLoad(imgOutput, pixelCoords).rgb;
        vec3 accumulatedColor = mix(oldColor, finalColor, 1.0 / float(frameCount));

        imageStore(imgOutput, pixelCoords, vec4(accumulatedColor, 1.0));
    }
}