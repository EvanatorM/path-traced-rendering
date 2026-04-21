#version 450 core

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

// The noisy 10-spp or 100-spp image from your path tracer
layout(rgba32f, binding = 0) uniform readonly image2D inputImage;

// The final smoothed image to draw to the screen
layout(rgba32f, binding = 1) uniform writeonly image2D outputImage;

// Filter tuning parameters
#define SEARCH_RADIUS 4      // How far out to sample (a 9x9 grid)
#define SPATIAL_SIGMA 0.5    // Controls the falloff of the physical distance blur
#define COLOR_SIGMA 0.5      // Controls how strict edge detection is (lower = sharper edges, less blur)

void main() 
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 imgSize = imageSize(inputImage);
    
    if (pixelCoords.x >= imgSize.x || pixelCoords.y >= imgSize.y) return;

    // Fetch the color of the pixel we are currently trying to denoise
    vec3 centerColor = imageLoad(inputImage, pixelCoords).rgb;

    vec3 finalColor = vec3(0.0);
    float totalWeight = 0.0;

    // Loop through the surrounding grid of pixels
    for (int y = -SEARCH_RADIUS; y <= SEARCH_RADIUS; y++) 
    {
        for (int x = -SEARCH_RADIUS; x <= SEARCH_RADIUS; x++) 
        {
            ivec2 offset = ivec2(x, y);
            ivec2 sampleCoords = clamp(pixelCoords + offset, ivec2(0), imgSize - 1);
            vec3 sampleColor = imageLoad(inputImage, sampleCoords).rgb;

            // 1. Calculate Spatial Weight (Standard Gaussian based on physical distance)
            float spatialDist2 = float(x * x + y * y);
            float spatialWeight = exp(-spatialDist2 / (2.0 * SPATIAL_SIGMA * SPATIAL_SIGMA));

            // 2. Calculate Color Weight (Edge-stopping function based on color difference)
            vec3 colorDiff = centerColor - sampleColor;
            float colorDist2 = dot(colorDiff, colorDiff);
            float colorWeight = exp(-colorDist2 / (2.0 * COLOR_SIGMA * COLOR_SIGMA));

            // Combine both weights
            float weight = spatialWeight * colorWeight;

            // Accumulate the weighted color
            finalColor += sampleColor * weight;
            totalWeight += weight;
        }
    }

    // Normalize the accumulated color by the total weight
    finalColor /= totalWeight;

    // Optional: Add a simple Gamma Correction (2.2) to make the lighting look much more realistic 
    // before displaying it to your monitor.
    finalColor = pow(finalColor, vec3(1.0 / 2.2));

    imageStore(outputImage, pixelCoords, vec4(finalColor, 1.0));
}