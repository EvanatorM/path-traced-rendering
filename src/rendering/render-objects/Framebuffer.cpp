#include <rendering/render-objects/Framebuffer.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

Framebuffer::Framebuffer()
{
    glGenFramebuffers(1, &_framebufferId);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &_framebufferId);
}

void Framebuffer::SaveToFile(const std::string& filePath, int width, int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

    std::vector<unsigned char> pixels(width * height * 4);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    stbi_flip_vertically_on_write(false);

    int stride_in_bytes = width * 4;
    int success = stbi_write_png(filePath.c_str(), width, height, 4, pixels.data(), stride_in_bytes);

    if (success)
        std::cout << "Saved image\n";
    else
        std::cout << "Failed to save image\n";
}

void Framebuffer::StartRender(const Texture& texture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.GetTextureId(), 0);
    glViewport(0, 0, texture.GetWidth(), texture.GetHeight());
}