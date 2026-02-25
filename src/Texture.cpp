#include <Texture.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>

Texture::Texture(int width, int height)
{
    glGenTextures(1, &_textureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindImageTexture(0, _textureId, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

Texture::~Texture()
{
    glDeleteTextures(1, &_textureId);
}

void Texture::Bind(int slot)
{
    switch (slot)
    {
        case 0:
            glActiveTexture(GL_TEXTURE0);
            break;
        case 1:
            glActiveTexture(GL_TEXTURE1);
            break;
        case 2:
            glActiveTexture(GL_TEXTURE2);
            break;
        case 3:
            glActiveTexture(GL_TEXTURE3);
            break;
        case 4:
            glActiveTexture(GL_TEXTURE4);
            break;
        case 5:
            glActiveTexture(GL_TEXTURE5);
            break;
        case 6:
            glActiveTexture(GL_TEXTURE6);
            break;
        case 7:
            glActiveTexture(GL_TEXTURE7);
            break;
        case 8:
            glActiveTexture(GL_TEXTURE8);
            break;
    }
    glBindTexture(GL_TEXTURE_2D, _textureId);
}