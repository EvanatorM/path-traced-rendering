#include <PathTracer.h>

namespace PathTracer
{
    void PathTrace(const Scene& scene, Image& image)
    {
        int r = 0;
        int b = 0;
        for (int x = 0; x < image.width; x++)
        {
            for (int y = 0; y < image.height; y++)
            {
                image.SetPixel(x, y, r % 256, 0, b % 256);
                r += 1;
            }

            b += 1;
            r = 0;
        }
    }
}