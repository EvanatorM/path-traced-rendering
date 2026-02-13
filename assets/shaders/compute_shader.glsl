#version 450 core

layout(local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform float t;

bool intersectSphere(vec3 pos, vec3 rot, vec3 rayOrigin, vec3 rayDir, )

void main()
{
    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    float speed = 100;
    float width = 1000;
    float height = 1000;

    float worldX = 2 * (texelCoord.x / width) - 1;
    float worldY = 2 * (texelCoord.y / height) - 1;

    float nearestT = 10000.0;
    int nearestObject = -1;

    value.x = mod(float(texelCoord.x) + t * speed, width) / (gl_NumWorkGroups.x * gl_WorkGroupSize.x);
    value.y = float(texelCoord.y) / (gl_NumWorkGroups.y * gl_WorkGroupSize.y);

    imageStore(imgOutput, texelCoord, value);
}