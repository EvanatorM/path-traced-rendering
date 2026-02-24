#include <Mesh.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

Mesh::Mesh()
{
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);
}

Mesh::Mesh(Vertex* vertices, int vertexCount, uint32_t* indices, int indexCount)
{
    _indexCount = indexCount;
    
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);
}

std::vector<std::string> SplitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    
    tokens.push_back(str.substr(start));
    return tokens;
}

Mesh Mesh::CreateFromFile(const std::string& meshFile)
{
    // Open file
    std::ifstream modelFile(meshFile);

    std::string line;
    std::vector<glm::vec3> vPos;
    std::vector<glm::vec2> vTexPos;
    std::vector<glm::vec3> vNorm;
    std::vector<std::string> faces;
    while (std::getline(modelFile, line))
    {
        if (line.rfind("v ", 0) == 0)
        {
            auto tokens = SplitString(line, ' ');
            vPos.emplace_back(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
        }
        else if (line.rfind("vt ", 0) == 0)
        {
            auto tokens = SplitString(line, ' ');
            vTexPos.emplace_back(std::stof(tokens[1]), std::stof(tokens[2]));
        }
        else if (line.rfind("vn ", 0) == 0)
        {
            auto tokens = SplitString(line, ' ');
            vNorm.emplace_back(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
        }
        else if (line.rfind("f ", 0) == 0)
        {
            faces.push_back(line);
        }
    }

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    for (auto& f : faces)
    {
        //Logger::Log("Face: %s", f.c_str());
        // Split by vertices in face
        auto tokens = SplitString(f, ' ');
        if (tokens.size() == 4) // Triangle
        {
            indices.push_back(vertices.size());
            indices.push_back(vertices.size() + 2);
            indices.push_back(vertices.size() + 1);

            for (int i = 1; i < tokens.size(); i++)
            {
                // Split by / to get values
                // Format: [vertex index]/[texPos index]/[normal index]
                // Index starts at 1 in obj, not 0
                auto vProps = SplitString(tokens[i], '/');

                //Logger::Log("Vertex: v:%s t:%s n:%s", vProps[0].c_str(), vProps[1].c_str(), vProps[2].c_str());
                vertices.emplace_back(vPos[std::stoi(vProps[0]) - 1], vNorm[std::stoi(vProps[2]) - 1], vTexPos[std::stoi(vProps[1]) - 1]);
            }
        }
        else if (tokens.size() == 5) // Quads
        {
            indices.push_back(vertices.size());
            indices.push_back(vertices.size() + 2);
            indices.push_back(vertices.size() + 1);

            indices.push_back(vertices.size());
            indices.push_back(vertices.size() + 3);
            indices.push_back(vertices.size() + 2);

            for (int i = 1; i < tokens.size(); i++)
            {
                // Split by / to get values
                // Format: [vertex index]/[texPos index]/[normal index]
                // Index starts at 1 in obj, not 0
                auto vProps = SplitString(tokens[i], '/');

                // Since it's a quad, we need to split it into 2 triangles
                //Logger::Log("Vertex: v:%s t:%s n:%s", vProps[0].c_str(), vProps[1].c_str(), vProps[2].c_str());

                vertices.emplace_back(vPos[std::stoi(vProps[0]) - 1], vNorm[std::stoi(vProps[2]) - 1], vTexPos[std::stoi(vProps[1]) - 1]);
            }
        }
        else // Unsupported format
        {
            std::cerr << "Unsupported model type!\n";
            return Mesh();
        }
    }

    return Mesh(vertices.data(), vertices.size(), indices.data(), indices.size());
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
    glDeleteVertexArrays(1, &_vao);
}

void Mesh::BufferData(Vertex* vertices, int vertexCount, uint32_t* indices, int indexCount)
{
    _indexCount = indexCount;

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);
}

void Mesh::Draw()
{
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);
}