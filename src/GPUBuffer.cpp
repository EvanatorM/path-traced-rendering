#include <GPUBuffer.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>

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
    glNamedBufferStorage(_bufferId, dataSize, data, GL_DYNAMIC_STORAGE_BIT);
}

void GPUBuffer::Bind(int binding)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, _bufferId);
}