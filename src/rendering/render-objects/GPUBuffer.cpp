#include <rendering/render-objects/GPUBuffer.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

GPUBuffer::GPUBuffer()
{
    glCreateBuffers(1, &_bufferId);
}

GPUBuffer::~GPUBuffer()
{
    glDeleteBuffers(1, &_bufferId);
}

void GPUBuffer::BufferData(const void* data, size_t dataSize)
{
    glNamedBufferData(_bufferId, static_cast<GLsizeiptr>(dataSize), data, GL_DYNAMIC_DRAW);
}

void GPUBuffer::Bind(int binding)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, _bufferId);
}

unsigned int GPUBuffer::GetId() const
{
    return _bufferId;
}