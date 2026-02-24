#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <string>

struct Vertex
{
    glm::vec3 position;
    glm::vec2 texCoords;
};

class Mesh
{
private:
    unsigned int _vbo, _vao, _ebo;
    uint32_t _indexCount;

public:
    Mesh();
    Mesh(Vertex* vertices, int vertexCount, uint32_t* indices, int indexCount);

    static Mesh CreateFromFile(const std::string& meshFile);

    ~Mesh();

    void BufferData(Vertex* vertices, int vertexCount, uint32_t* indices, int indexCount);
    void Draw();
};