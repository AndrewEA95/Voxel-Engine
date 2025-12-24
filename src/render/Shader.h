#pragma once

#include <string>
#include <unordered_map>

#include "../../extern/glm-1.0.2/glm/glm.hpp"

namespace Render
{
    class Shader
    {
        public:
            Shader(const std::string& vertexPath, const std::string& fragmentPath);
            ~Shader();

            void bind() const;
            void unbind() const;

            // Uniform setters
            void setUniformMat4(const std::string& name, const glm::mat4& matrix);
            void setUniformVec3(const std::string& name, const glm::vec3& cector);
            void setUniformFloat(const std::string& name, float value);
            void setUniformInt(const std::string& name, int value);

        private:
            uint32_t m_programID;

            std::string loadFile(const std::string& path);
            uint32_t compileShader(uint32_t type, const std::string& source);
            uint32_t linkProgram(uint32_t vertextShader, uint32_t fragmentShader);

            int getUniformLocation(const std::string& name);

            std::unordered_map<std::string, int> m_uniformLocationCache;
    };
}