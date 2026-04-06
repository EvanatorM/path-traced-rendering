#pragma once

#include <string>

class Texture
{
private:
    unsigned int _textureId;
    int _width, _height;

public:
    Texture(int width, int height);
    ~Texture();

    void Bind(int slot);

    unsigned int GetTextureId() const { return _textureId; }
    int GetWidth() const { return _width; }
    int GetHeight() const { return _height; }
};