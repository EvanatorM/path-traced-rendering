#pragma once

#include <string>

class Texture
{
private:
    unsigned int _textureId;

public:
    Texture(int width, int height);
    ~Texture();

    void Bind(int slot);
};