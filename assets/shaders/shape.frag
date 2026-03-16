#version 450 core

#define M_PI 3.1415926535897932384626433832795

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform vec3 color;

out vec4 FragColor;

struct PointLight {
    vec3 position;
    float attenuation;
    vec3 color;
    float intensity;
};

struct AreaSphereLight {
    vec3 position;
    float attenuation;
    vec3 color;
    float intensity;
    vec3 padding;
    float radius;
};

layout(std430, binding = 3) buffer pointLightBuffer
{
    PointLight[] pointLights;
};
uniform int numPointLights;

layout(std430, binding = 5) buffer areaSphereLightBuffer
{
    AreaSphereLight[] areaSphereLights;
};
uniform int numAreaSphereLights;

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos)
{
    vec3 toLight = light.position - fragPos;
    float dist2 = max(dot(toLight, toLight), 1e-6);
    vec3 L = toLight * inversesqrt(dist2);

    float NdotL = max(dot(normalize(normal), L), 0.0);
    vec3 radiance = light.color * light.intensity / (dist2 / light.attenuation);

    return radiance * NdotL;
}

vec3 CalculateAreaSphereLight(AreaSphereLight light, vec3 normal, vec3 fragPos)
{
    vec3 toLight = light.position - fragPos;
    float dist2 = max(dot(toLight, toLight), 1e-6);
    vec3 L = toLight * inversesqrt(dist2);

    float NdotL = max(dot(normalize(normal), L), 0.0);
    vec3 radiance = light.color * light.intensity / (dist2 / light.attenuation);

    return radiance * NdotL;
}

void main()
{    
    vec3 N = normalize(Normal);

    vec3 lighting = vec3(0.0);
    for (int i = 0; i < numPointLights; i++)
    {
        lighting += CalculatePointLight(pointLights[i], N, FragPos);
    }
    for (int i = 0; i < numAreaSphereLights; i++)
    {
        lighting += CalculateAreaSphereLight(areaSphereLights[i], N, FragPos);
    }

    FragColor = vec4(lighting * (color / M_PI), 1.0);
}