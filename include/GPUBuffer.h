#pragma once

class GPUBuffer
{
private:
    unsigned int _bufferId;

public:
    GPUBuffer();
    ~GPUBuffer();

    void BufferData(const void* data, size_t dataSize);

    void Bind(int binding);
};