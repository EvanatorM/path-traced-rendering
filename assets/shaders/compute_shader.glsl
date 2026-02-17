#version 450 core

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

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
    if (h < 0.0) return false;
    t = -b - sqrt(h);
    return t > 0.0;
}

Sphere spheres[] = {
    Sphere(vec3(0.0, 0.0, -2.0), 1.0, vec4(1.0, 1.0, 1.0, 1.0))
};

void main()
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    vec2 size = vec2(1024, 1024);

    // Ray Generation
    vec3 origin = vec3(
        (float(pixelCoords.x) / size.x) * 3.0 - 1.5, 
        (float(pixelCoords.y) / size.y) * 3.0 - 1.5, 
        0.0
    );
    
    // The Intersection Loop
    float nearestT = 1e20;
    vec3 finalColor = vec3(0.0); // Background

    for (int i = 0; i < spheres.length(); i++)
    {
        float t;
        if (intersectSphere(origin, vec3(0.0, 0.0, -1.0), spheres[i], t))
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