#pragma once

#include <rendering/render-objects/Texture.h>
#include <string>

class Framebuffer
{
private:
    unsigned int _framebufferId;

public:
    Framebuffer();
    ~Framebuffer();

    void SaveToFile(const std::string& filePath, int width, int height);

    void StartRender(const Texture& texture);
};