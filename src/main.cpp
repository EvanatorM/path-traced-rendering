#include <Scene.h>
#include <Sphere.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

int main()
{
    Scene scene;
    scene.AddObject(Sphere{ 1.0f });

    std::vector<unsigned char> data = {
        255, 0, 0,   // Red pixel
        0, 255, 0,   // Green pixel
        0, 0, 255,   // Blue pixel
        255, 255, 0  // Yellow pixel
    };
    uint32_t imageSize = data.size();

    std::ofstream file("output.ppm", std::ios::binary);
    if (!file)
    {
        std::cerr << "Error creating file\n";
        return 1;
    }

    // https://my.eng.utah.edu/~cs5610/ppm.html
    std::string header = "P6\n2 2\n255\n";

    file.write(reinterpret_cast<const char*>(header.data()), header.size());
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    file.close();

    std::cout << "Image successfully written to output.ppm\n";
}