#pragma once

#include <glm/glm.hpp>

class Shader
{
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void Bind();

    void SetBool(const char* name, bool value) const;
    void SetInt(const char* name, int value) const;
    void SetFloat(const char* name, float value) const;
    void SetVec2(const char* name, glm::vec2 value) const;
    void SetVec2(const char* name, float x, float y) const;
    void SetVec3(const char* name, glm::vec3 value) const;
    void SetVec3(const char* name, float x, float y, float z) const;
    void SetVec4(const char* name, glm::vec4 value) const;
    void SetVec4(const char* name, float x, float y, float z, float w) const;
    void SetMat4(const char* name, glm::mat4 value) const;

private:
    unsigned int _programId;
};