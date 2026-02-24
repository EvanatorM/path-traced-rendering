#version 450 core

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform vec3 color;

out vec4 FragColor;

struct PointLight {
    vec4 position;

    float constant;
    float linear;
    float quadratic;
    float padding1;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

layout(std430, binding = 3) buffer pointLightBuffer
{
    PointLight[] pointLights;
};
uniform int numPointLights;

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    // attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // combine results
    vec3 ambient = light.ambient.xyz * vec3(color);
    vec3 diffuse = light.diffuse.xyz * diff * vec3(color);
    vec3 specular = light.specular.xyz * spec * vec3(1.0); // Assuming white specular highlights

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);
    for (int i = 0; i < numPointLights; i++)
    {
        result += CalculatePointLight(pointLights[i], Normal, FragPos, viewDir);
    }

    FragColor = vec4(result, 1.0);
}