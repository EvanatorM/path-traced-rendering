#include <Shader.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "Error reading shader source files: " << e.what() << '\n';
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "Error compiling vertex shader! (" << vertexPath << "): " << infoLog << '\n';
    }

    // fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cout << "Error compiling fragment shader! (" << fragmentPath << "): " << infoLog << '\n';
    }

    // shader program
    _programId = glCreateProgram();
    glAttachShader(_programId, vertex);
    glAttachShader(_programId, fragment);
    glLinkProgram(_programId);
    // print linking errors if any
    glGetProgramiv(_programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(_programId, 512, nullptr, infoLog);
        std::cout << "Error linking shader program: " << infoLog << '\n';
    }

    // delete the shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    glDeleteProgram(_programId);
}

void Shader::Bind()
{
    glUseProgram(_programId);
}

void Shader::SetBool(const char* name, bool value) const
{
    glUniform1i(glGetUniformLocation(_programId, name), (int)value);
}

void Shader::SetInt(const char* name, int value) const
{
    glUniform1i(glGetUniformLocation(_programId, name), value);
}

void Shader::SetFloat(const char* name, float value) const
{
    glUniform1f(glGetUniformLocation(_programId, name), value);
}

void Shader::SetVec2(const char* name, glm::vec2 value) const
{
    glUniform2f(glGetUniformLocation(_programId, name), value.x, value.y);
}

void Shader::SetVec2(const char* name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(_programId, name), x, y);
}

void Shader::SetVec3(const char* name, glm::vec3 value) const
{
    glUniform3f(glGetUniformLocation(_programId, name), value.x, value.y, value.z);
}

void Shader::SetVec3(const char* name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(_programId, name), x, y, z);
}

void Shader::SetVec4(const char* name, glm::vec4 value) const
{
    glUniform4f(glGetUniformLocation(_programId, name), value.x, value.y, value.z, value.w);
}

void Shader::SetVec4(const char* name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(_programId, name), x, y, z, w);
}

void Shader::SetMat4(const char* name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(_programId, name), 1, GL_FALSE, glm::value_ptr(value));
}