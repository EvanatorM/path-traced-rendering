#include <ComputeShader.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <iostream>

ComputeShader::ComputeShader(const char* computePath)
{
    // Get compute shader source code
    std::string cCode;
    std::ifstream cShaderFile;
    cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        cShaderFile.open(computePath);
        std::stringstream cShaderStream;
        cShaderStream << cShaderFile.rdbuf();
        cShaderFile.close();
        cCode = cShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << computePath << std::endl;
    }

    unsigned int compute;
    compute = glCreateShader(GL_COMPUTE_SHADER);

    const char* cShaderCode = cCode.c_str();
    glShaderSource(compute, 1, &cShaderCode, NULL);
    glCompileShader(compute);

    // Check for errors
    int success;
    char infoLog[512];
    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(compute, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    _programId = glCreateProgram();
    glAttachShader(_programId, compute);
    glLinkProgram(_programId);
    glDeleteShader(compute);
    // Check for linking errors
    glGetProgramiv(_programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(_programId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

ComputeShader::~ComputeShader()
{
    glDeleteProgram(_programId);
}

void ComputeShader::Bind()
{
    glUseProgram(_programId);
}

void ComputeShader::SetBool(const char* name, bool value) const
{
    glUniform1i(glGetUniformLocation(_programId, name), (int)value);
}

void ComputeShader::SetInt(const char* name, int value) const
{
    glUniform1i(glGetUniformLocation(_programId, name), value);
}

void ComputeShader::SetFloat(const char* name, float value) const
{
    glUniform1f(glGetUniformLocation(_programId, name), value);
}

void ComputeShader::SetVec2(const char* name, glm::vec2 value) const
{
    glUniform2f(glGetUniformLocation(_programId, name), value.x, value.y);
}

void ComputeShader::SetVec2(const char* name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(_programId, name), x, y);
}

void ComputeShader::SetVec3(const char* name, glm::vec3 value) const
{
    glUniform3f(glGetUniformLocation(_programId, name), value.x, value.y, value.z);
}

void ComputeShader::SetVec3(const char* name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(_programId, name), x, y, z);
}

void ComputeShader::SetVec4(const char* name, glm::vec4 value) const
{
    glUniform4f(glGetUniformLocation(_programId, name), value.x, value.y, value.z, value.w);
}

void ComputeShader::SetVec4(const char* name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(_programId, name), x, y, z, w);
}

void ComputeShader::SetMat4(const char* name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(_programId, name), 1, GL_FALSE, glm::value_ptr(value));
}