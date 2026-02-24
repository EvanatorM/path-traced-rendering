#version 450 core

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform vec3 color;

out vec4 FragColor;

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // combine results
    vec3 ambient = light.ambient * vec3(color);
    vec3 diffuse = light.diffuse * diff * vec3(color);
    vec3 specular = light.specular * spec * vec3(1.0); // Assuming white specular highlights

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
    PointLight light;
    light.position = vec3(0.0, 0.0, 0.0);
    light.ambient = vec3(0.1, 0.1, 0.1);
    light.diffuse = vec3(0.8, 0.8, 0.8);
    light.specular = vec3(1.0, 1.0, 1.0);
    light.constant = 1.0;
    light.linear = 0.09;
    light.quadratic = 0.032;

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);
    result += CalculatePointLight(light, Normal, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}