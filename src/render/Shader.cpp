#define GL_SILENCE_DEPRECATION

#include "Shader.h"
#include "../core/Log.h"

#include <OpenGL/gl.h>
#include <fstream>
#include <sstream>
#include "../../extern/glm-1.0.2/glm/gtc/type_ptr.hpp"
//#include  "../../extern/glad/include/glad/glad.h"

namespace Render
{
    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    {
        std::string vertexSrc = loadFile(vertexPath);
        std::string fragmentSrc = loadFile(fragmentPath);

        uint32_t vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
        uint32_t fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

        m_programID = linkProgram(vertexShader, fragmentShader);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_programID);
    }

    void Shader::bind() const
    {
        glUseProgram(m_programID);
    }

    void Shader::unbind() const
    {
        glUseProgram(0);
    }

    std::string Shader::loadFile(const std::string& path)
    {
        std::ifstream file(path);
        if(!file.is_open())
        {
            LOG_ENGINE_ERROR("Failed to open shader file: " + path);
            return "";
        }

        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    uint32_t Shader::compileShader(uint32_t type, const std::string& source)
    {
        uint32_t shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if(!success)
        {
            char infoLog[1024];
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);

            LOG_ENGINE_ERROR(
                std::string("Shader comilation failed") +
                (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") +
                "\n" + infoLog
            );
        }

        return shader;
    }

    uint32_t Shader::linkProgram(uint32_t vertexShader, uint32_t fragmentShader)
    {
        uint32_t program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program,  fragmentShader);
        glLinkProgram(program);

        int success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if(!success)
        {
            char infoLog[1024];
            glGetProgramInfoLog(program, 1024, nullptr, infoLog);

            LOG_ENGINE_ERROR("Shader linking failed:\n" + std::string(infoLog));
        }

        return program;
    }

    int Shader::getUniformLocation(const std::string& name)
    {
        auto it = m_uniformLocationCache.find(name);
        if(it != m_uniformLocationCache.end())
        {
            return it->second;
        }

        int location = glGetUniformLocation(m_programID, name.c_str());
        if(location == -1)
        {
            LOG_ENGINE_WARN("Uniform not found: " + name);
        }

        m_uniformLocationCache[name] = location;
        return location;
    }

    void Shader::setUniformMat4(const std::string& name, const glm::mat4& matrix)
    {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void Shader::setUniformVec3(const std::string& name, const glm::vec3& vector)
    {
        glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(vector));
    }

    void Shader::setUniformFloat(const std::string& name, float value)
    {
        glUniform1f(getUniformLocation(name), value);
    }

    void Shader::setUniformInt(const std::string& name, int value)
    {
        glUniform1i(getUniformLocation(name), value);
    }
}