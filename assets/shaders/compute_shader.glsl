#version 450 core

#define M_PI 3.1415926535897932384626433832795
#define MAX_BOUNCES 8
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

struct Material {
    vec4 albedo_roughness;
    vec4 emission_metallic;
};

layout(std430, binding = 6) buffer materialBuffer
{
    Material[] materials;
};

struct Sphere {
    vec3 center;
    float radius;
    uint matIndex;
    vec3 padding;
};

struct Plane {
    vec3 offset;
    uint matIndex;
    vec3 orientation;
    float padding;
};

struct Cube {
    vec4 position;
    vec3 size;
    uint matIndex;
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
    uint matIndex;
    vec3 color;
    float area;
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
// "PCG random number generators in glsl" by Riccardo (https://observablehq.com/@riccardoscalco/pcg-random-number-generators-in-glsl)
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
// "A Minimal Ray-Tracer: Ray-Sphere Intersection" by Jean-Colas Prunier (https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html)
bool intersectSphere(vec3 ro, vec3 rd, Sphere s, out float t, out vec3 normal)
{
    vec3 L = s.center - ro;
    float tca = dot(L, rd);
    if (tca < 0) return false;
    float d2 = dot(L, L) - tca * tca;
    if (d2 > s.radius * s.radius) return false;
    float thc = sqrt(s.radius * s.radius - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;

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
// "A Minimal Ray-Tracer: Ray-Plane and Ray-Disk Intersection" by Jean-Colas Prunier (https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html)
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
// "An Efficient and Robust Ray–Box Intersection Algorithm" by Amy Williams et al. (https://people.csail.mit.edu/amy/papers/box-jgt.pdf)
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

bool getSceneIntersection(vec3 ro, vec3 rd, out float closestT, out vec3 normal, out uint matIndex, out bool hitLight, out float hitLightArea)
{
    closestT = 1e20;
    bool hit = false;
    hitLight = false;
    hitLightArea = 0.0;
    float t;
    vec3 n;

    for (int i = 0; i < numSpheres; i++)
    {
        if (intersectSphere(ro, rd, spheres[i], t, n) && t < closestT)
        {
            closestT = t;
            normal = n;
            matIndex = spheres[i].matIndex;
            hit = true;
        }
    }
    for(int i = 0; i < numPlanes; i++)
    {
        if(intersectPlane(ro, rd, planes[i], t, n) && t < closestT)
        {
            closestT = t;
            normal = n;
            matIndex = planes[i].matIndex;
            hit = true;
        }
    }
    for(int i = 0; i < numCubes; i++)
    {
        if(intersectCube(ro, rd, cubes[i], t, n) && t < closestT)
        {
            closestT = t;
            normal = n;
            matIndex = cubes[i].matIndex;
            hit = true;
        }
    }
    for (int i = 0; i < numQuadLights; i++)
    {
        if (intersectQuad(ro, rd, quadLights[i], t, n) && t < closestT)
        {
            closestT = t;
            normal = n;
            matIndex = quadLights[i].matIndex;
            hitLight = true;
            hitLightArea = quadLights[i].area;
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

// Evaluates the distribution of visible normals.
// Based on algorithm by Dupuy & Benyoub (2023)
vec3 sampleVndf_GGX(vec2 u, vec3 wi, float alpha, vec3 n)
{
    vec3 wi_z = n * dot(wi, n);
    vec3 wi_xy = wi - wi_z;
    vec3 wiStd = normalize(wi_z - alpha * wi_xy);

    float wiStd_z = dot(wiStd, n);
    float phi = (2.0 * u.x - 1.0) * M_PI;
    float z = (1.0 - u.y) * (1.0 + wiStd_z) - wiStd_z;
    float sinTheta = sqrt(clamp(1.0 - z * z, 0.0, 1.0));

    vec3 cStd = vec3(sinTheta * cos(phi), sinTheta * sin(phi), z);
    vec3 up = vec3(0.0, 0.0, 1.0);
    vec3 wr = n + up;
    vec3 c = dot(wr, cStd) * wr / wr.z - cStd;

    vec3 wmStd = c + wiStd;
    vec3 wmStd_z = n * dot(n, wmStd);
    vec3 wmStd_xy = wmStd_z - wmStd;

    return normalize(wmStd_z + alpha * wmStd_xy);
}

// Trowbridge-Reitz (GGX) Normal Distribution Function
float D_GGX(float NdotH, float alpha)
{
    float a2 = alpha * alpha;
    float denom = (NdotH * NdotH * (a2 - 1.0) + 1.0);
    return a2 / (M_PI * denom * denom);
}

// Smith Correlated Masking-Shadowing Function
float G_Smith(float NdotL, float NdotV, float alpha)
{
    float a2 = alpha * alpha;
    float gl = NdotL + sqrt(a2 + (1.0 - a2) * NdotL * NdotL);
    float gv = NdotV + sqrt(a2 + (1.0 - a2) * NdotV * NdotV);
    return 1.0 / (gl * gv);
}

// Schlick's Approximation of Fresnel Reflectance
vec3 F_Schlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Probability Density Function corresponding to the GGX VNDF
float PDF_VNDF_GGX(float NdotH, float NdotV, float alpha)
{
    float D = D_GGX(NdotH, alpha);
    float G1 = 2.0 * NdotV / (NdotV + sqrt(alpha * alpha + (1.0 - alpha * alpha) * NdotV * NdotV));
    return (D * G1) / (4.0 * NdotV);
}

// Multiple Importance Sampling (Veach Balance Heuristic)
// Combines the PDFs of light sampling and BRDF sampling to minimize variance
float balanceHeuristic(float pdfA, float pdfB)
{
    return pdfA / (pdfA + pdfB);
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

    // State trackers for MIS
    float lastPdfBrdf = 1.0;
    bool lastBounceSpecular = true;

    for (int bounce = 0; bounce < MAX_BOUNCES; bounce++)
    {
        float t;
        vec3 normal;
        uint matIndex;
        bool hitLight;
        float hitLightArea;

        if (!getSceneIntersection(currentRayOrigin, currentRayDir, t, normal, matIndex, hitLight, hitLightArea))
        {
            radiance += throughput * backgroundColor;
            break;
        }

        // Get material properties
        Material mat = materials[matIndex];
        vec3 albedo = mat.albedo_roughness.rgb;
        float roughness = max(mat.albedo_roughness.a, 0.001);
        vec3 emission = mat.emission_metallic.rgb;
        float metallic = mat.emission_metallic.a;

        // 1. Evaluate Emission (if hit light source)
        if (hitLight || length(emission) > 0.0)
        {
            // Only add emission if it's the first camera ray
            if (bounce == 0)
                radiance += throughput * emission;
            else
            {
                // Resolve MIS: Weigh probability of the BRDF generating this ray
                // against the probability that light sampling could have generated it.
                float lightCos = max(dot(normal, -currentRayDir), 0.001);

                float lightPdf = (t * t) / (hitLightArea * lightCos);
                float weight = balanceHeuristic(lastPdfBrdf, lightPdf);
                radiance += throughput * emission * weight;
            }

            break; // Stop tracing when light is hit
        }

        vec3 hitPoint = currentRayOrigin + currentRayDir * t;
        vec3 V = normalize(-currentRayDir);
        float NdotV = max(dot(normal, V), 0.001);

        // Base reflectivity for dielectrics is 4%, for metals it inherits the albedo color
        vec3 F0 = mix(vec3(0.04), albedo, metallic);
        float alpha = roughness * roughness;

        // 2. Direct Illumination
        vec3 directLighting = vec3(0.0);

        // Point Lights (does not use MIS)
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
                    float attenuation = 1.0 / (dist2 + pointLights[i].attenuation * pointLights[i].attenuation);
                    vec3 lightIntensity = pointLights[i].color * pointLights[i].intensity * attenuation;

                    vec3 H = normalize(V + L);
                    vec3 F = F_Schlick(max(dot(H, V), 0.0), F0);
                    vec3 kS = F;
                    vec3 kD = (1.0 - kS) * (1.0 - metallic);

                    float D = D_GGX(max(dot(normal, H), 0.0), alpha);
                    float G = G_Smith(NdotL, NdotV, alpha);
                    vec3 specular = (D * G * F) / (4.0 * NdotL * NdotV + 0.001);
                    
                    directLighting += lightIntensity * NdotL * (kD * albedo / M_PI + specular);
                }
            }
        }
        // Quad Lights (uses MIS weighting)
        for (int i = 0; i < numQuadLights; i++)
        {
            vec2 r = vec2(randomFloat(), randomFloat());
            vec3 lightPoint = quadLights[i].position + (quadLights[i].u * r.x) + (quadLights[i].v * r.y);
            vec3 toLight = lightPoint - hitPoint;
            float dist2 = dot(toLight, toLight);
            float dist = sqrt(dist2);
            vec3 L = toLight / dist;
            float NdotL = max(dot(normal, L), 0.0);

            // Check if light is hitting the front of the surface
            if (NdotL > 0.0)
            {
                // Calculate the normal of the light
                vec3 lightNormal = normalize(cross(quadLights[i].u, quadLights[i].v));
                float lightCos = max(dot(lightNormal, -L), 0.0);

                // Check if the surface is facing the front of the light
                if (lightCos > 0.0)
                {
                    // Cast shadow ray
                    if (!rayBlocked(hitPoint + normal * BIAS, L, dist))
                    {
                        // Get PDF
                        float pdfLight = dist2 / (quadLights[i].area * lightCos);

                        // BRDF Evaluation
                        vec3 H = normalize(V + L);
                        float NdotH = max(dot(normal, H), 0.0);
                        vec3 F = F_Schlick(max(dot(H, V), 0.0), F0);
                        vec3 kS = F;
                        vec3 kD = (1.0 - kS) * (1.0 - metallic);

                        float D = D_GGX(NdotH, alpha);
                        float G = G_Smith(NdotL, NdotV, alpha);
                        vec3 specular = (D * G * F) / (4.0 * NdotL * NdotV + 0.001);
                        vec3 brdf = (kD * albedo / M_PI) + specular;

                        // Calculate the BRDF PDF for the MIS weight
                        float pdfBrdfDiffuse = NdotL / M_PI;
                        float pdfBrdfSpecular = PDF_VNDF_GGX(NdotH, NdotV, alpha);

                        // Branchless probability interpolation based on metallicity
                        float probabilitySpecular = mix(0.5, 1.0, metallic);
                        float pdfBrdf = mix(pdfBrdfDiffuse, pdfBrdfSpecular, probabilitySpecular);

                        float misWeight = balanceHeuristic(pdfLight, pdfBrdf);
                        vec3 lightIntensity = quadLights[i].color * quadLights[i].intensity;
                        directLighting += lightIntensity * brdf * NdotL * misWeight / pdfLight;
                    }
                }
            }
        }
        radiance += throughput * directLighting;

        // 3. Indirect Lighting (Monte Carlo)
        // Based on methods found in
        // "Global Illumination and Path Tracing: a Practical Implementation" by Jean-Colas Prunier https://www.scratchapixel.com/lessons/3d-basic-rendering/global-illumination-path-tracing/global-illumination-path-tracing-practical-implementation.html
        float probabilitySpecular = mix(0.5, 1.0, metallic);
        bool doSpecular = randomFloat() < probabilitySpecular;

        vec3 nextDir;
        if (doSpecular)
        {
            vec3 H = sampleVndf_GGX(vec2(randomFloat(), randomFloat()), V, alpha, normal);
            nextDir = reflect(-V, H);
            lastBounceSpecular = true;
        }
        else
        {
            vec2 r = vec2(randomFloat(), randomFloat());
            vec3 localDir = cosineSampleHemisphere(r.x, r.y);
            vec3 T, B;
            buildOrthonormalBasis(normal, T, B);
            nextDir = normalize(localDir.x * T + localDir.y * B + localDir.z * normal);
            lastBounceSpecular = false;
        }

        float NdotL = max(dot(normal, nextDir), 0.0);
        if (NdotL <= 0.0) break; // Path absorbed/occluded

        vec3 H = normalize(V + nextDir);
        float NdotH = max(dot(normal, H), 0.0);
        vec3 F = F_Schlick(max(dot(H, V), 0.0), F0);
        vec3 kS = F;
        vec3 kD = (1.0 - kS) * (1.0 - metallic);

        float D = D_GGX(NdotH, alpha);
        float G = G_Smith(NdotL, NdotV, alpha);
        vec3 specular = (D * G * F) / (4.0 * NdotL * NdotV + 0.001);
        vec3 diffuse = (kD * albedo / M_PI);

        float pdfDiffuse = NdotL / M_PI;
        float pdfSpecular = PDF_VNDF_GGX(NdotH, NdotV, alpha);
        lastPdfBrdf = mix(pdfDiffuse, pdfSpecular, probabilitySpecular);

        if (lastPdfBrdf <= 0.0) break;

        // Multiply throughput by BRDF. Divide by PDF
        throughput *= (diffuse + specular) * NdotL / lastPdfBrdf;
        currentRayDir = nextDir;
        currentRayOrigin = hitPoint + normal * BIAS;

        // 4. Unbiased Russian Roulette Path Termination
        // Eliminates fixed-depth mathematical bias by statistically terminating
        // paths based on accumulated energy and physically boosting the survivors
        if (bounce > 3)
        {
            float pSurvival = clamp(max(throughput.r, max(throughput.g, throughput.b)), 0.05, 0.95);
            if (randomFloat() > pSurvival) break;
            throughput *= 1.0 / pSurvival; // Boost energy to maintain unbiased integral
        }
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