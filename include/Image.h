#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class Image
{
public:
    Image(int width, int height) : width(width), height(height), _data(width * height * 3, 0) {}

    void SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
    {
        int index = (y * width + x) * 3;
        _data[index] = r;
        _data[index + 1] = g;
        _data[index + 2] = b;
    }

    void SaveToPPM(const std::string& filename) const
    {
        std::ofstream file(filename, std::ios::binary);
        if (!file)
        {
            std::cerr << "Error creating file\n";
            return;
        }

        // https://my.eng.utah.edu/~cs5610/ppm.html
        std::string header = "P6\n" + std::to_string(width) + ' ' + std::to_string(height) + "\n255\n";

        file.write(reinterpret_cast<const char*>(header.data()), header.size());
        file.write(reinterpret_cast<const char*>(_data.data()), _data.size());
        file.close();

        std::cout << "Image successfully written to " << filename << "\n";
    }

    int width;
    int height;

private:
    std::vector<unsigned char> _data;
};