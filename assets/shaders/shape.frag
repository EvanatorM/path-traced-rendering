#version 450 core

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform vec3 color;

out vec4 FragColor;

struct PointLight {
    vec4 position;
    vec3 color;
    float intensity;
};

layout(std430, binding = 3) buffer pointLightBuffer
{
    PointLight[] pointLights;
};
uniform int numPointLights;

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos)
{
    vec3 toLight = light.position.xyz - fragPos;
    float dist2 = max(dot(toLight, toLight), 1e-6);
    vec3 L = toLight * inversesqrt(dist2);

    float NdotL = max(dot(normalize(normal), L), 0.0);
    vec3 radiance = light.color * light.intensity / dist2;

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

    FragColor = vec4(lighting * color, 1.0);
}