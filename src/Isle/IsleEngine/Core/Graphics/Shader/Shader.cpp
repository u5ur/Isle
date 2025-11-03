// Shader.cpp
#include "Shader.h"
#include <fstream>
#include <sstream>

namespace Isle
{
    Shader::Shader()
    {
        m_ProgramId = glCreateProgram();
    }

    Shader::~Shader()
    {
        if (m_ProgramId)
        {
            glDeleteProgram(m_ProgramId);
        }
    }

    bool Shader::CheckShaderErrors(GLuint shader, const std::string& type)
    {
        GLint success;
        GLchar infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            ISLE_ERROR("SHADER_COMPILATION_ERROR (%s): %s\n", type.c_str(), infoLog);
            return false;
        }
        return true;
    }

    bool Shader::CheckProgramErrors(GLuint program)
    {
        GLint success;
        GLchar infoLog[1024];
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (!success)
        {
            glGetProgramInfoLog(program, 1024, NULL, infoLog);
            ISLE_ERROR("PROGRAM_LINKING_ERROR: %s\n", infoLog);
            return false;
        }
        return true;
    }

    GLuint Shader::CompileShader(GLenum type, const std::string& source)
    {
        GLuint shader = glCreateShader(type);
        const char* sourceCStr = source.c_str();

        glShaderSource(shader, 1, &sourceCStr, nullptr);
        glCompileShader(shader);

        std::string shaderType;
        switch (type)
        {
        case GL_VERTEX_SHADER: shaderType = "VERTEX"; break;
        case GL_FRAGMENT_SHADER: shaderType = "FRAGMENT"; break;
        case GL_GEOMETRY_SHADER: shaderType = "GEOMETRY"; break;
        case GL_COMPUTE_SHADER: shaderType = "COMPUTE"; break;
        default: shaderType = "UNKNOWN"; break;
        }

        if (!CheckShaderErrors(shader, shaderType))
        {
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    void Shader::DispatchCompute(GLuint groupsX, GLuint groupsY, GLuint groupsZ) const
    {
        glDispatchCompute(groupsX, groupsY, groupsZ);
    }

    bool Shader::LoadFromSource(GLenum type, const std::string& source)
    {
        GLuint shader = glCreateShader(type);
        const char* sourceCStr = source.c_str();

        glShaderSource(shader, 1, &sourceCStr, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            ISLE_ERROR("Shader compilation failed: %s\n", infoLog);
            glDeleteShader(shader);
            return false;
        }

        glAttachShader(m_ProgramId, shader);
        glDeleteShader(shader);
        return true;
    }

    bool Shader::LoadFromFile(GLenum type, std::string path)
    {
        std::string source;
        READ_FILE(path, source);

        if (source.empty())
        {
            ISLE_ERROR("Failed to read shader file: %s\n", path.c_str());
            return false;
        }

        source = ProcessIncludes(source, path);
        GLuint shader = CompileShader(type, source);
        if (shader == 0)
        {
            ISLE_ERROR("Failed to compile shader: %s\n", path.c_str());
            return false;
        }

        m_Path = path;
        glAttachShader(m_ProgramId, shader);
        glDeleteShader(shader);
        return true;
    }

    bool Shader::Link()
    {
        glLinkProgram(m_ProgramId);

        if (!CheckProgramErrors(m_ProgramId))
        {
            ISLE_ERROR("Failed to link shader program!\n");
            return false;
        }

        glValidateProgram(m_ProgramId);
        GLint validated;
        glGetProgramiv(m_ProgramId, GL_VALIDATE_STATUS, &validated);
        if (!validated)
        {
            GLchar infoLog[1024];
            glGetProgramInfoLog(m_ProgramId, 1024, NULL, infoLog);
            ISLE_ERROR("PROGRAM_VALIDATION_ERROR: %s\n", infoLog);
            return false;
        }

        return true;
    }

    std::string Shader::ProcessIncludes(std::string& source, const std::string base_path)
    {
        std::istringstream stream(source);
        std::ostringstream result;
        std::string line;

        while (std::getline(stream, line))
        {
            size_t includePos = line.find("#include");
            if (includePos != std::string::npos)
            {
                size_t firstQuote = line.find("\"", includePos);
                size_t secondQuote = line.find("\"", firstQuote + 1);

                if (firstQuote != std::string::npos && secondQuote != std::string::npos)
                {
                    std::string filename = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);

                    size_t lastSlash = base_path.find_last_of("/\\");
                    std::string includeDir = (lastSlash != std::string::npos) ? base_path.substr(0, lastSlash + 1) : "";
                    std::string fullPath = includeDir + filename;

                    std::string includeContent;
                    READ_FILE(fullPath, includeContent);

                    if (!includeContent.empty())
                    {
                        result << includeContent << "\n";
                    }
                    else
                    {
                        ISLE_WARN("Failed to include file: %s\n", fullPath.c_str());
                        result << line << "\n";
                    }
                }
                else
                {
                    result << line << "\n";
                }
            }
            else
            {
                result << line << "\n";
            }
        }

        return result.str();
    }


    void Shader::Bind()
    {
        glUseProgram(m_ProgramId);
    }

    GLint Shader::GetUniform(const std::string& name) const
    {
        return glGetUniformLocation(m_ProgramId, name.c_str());
    }

    void Shader::SetBool(const std::string& name, bool value) const
    {
        GLint location = GetUniform(name);
        if (location != -1)
        {
            glUniform1i(location, static_cast<int>(value));
        }
    }

    void Shader::SetInt(const std::string& name, int value) const
    {
        GLint location = GetUniform(name);
        if (location != -1)
        {
            glUniform1i(location, value);
        }
    }

    void Shader::SetUInt(const std::string& name, unsigned int value) const
    {
        GLint location = GetUniform(name);
        if (location != -1)
        {
            glUniform1ui(location, value);
        }
    }

    void Shader::SetFloat(const std::string& name, float value) const
    {
        GLint location = GetUniform(name);
        if (location != -1)
        {
            glUniform1f(location, value);
        }
    }

    void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
    {
        GLint location = GetUniform(name);
        if (location != -1)
        {
            glUniform2fv(location, 1, glm::value_ptr(value));
        }
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
    {
        GLint location = GetUniform(name);
        if (location != -1)
        {
            glUniform3fv(location, 1, glm::value_ptr(value));
        }
    }

    void Shader::SetIVec3(const std::string& name, const glm::ivec3& value) const
    {
        GLint location = GetUniform(name);
        if (location != -1)
        {
            glUniform3iv(location, 1, glm::value_ptr(value));
        }
    }

    void Shader::SetIVec2(const std::string& name, const glm::ivec2& value) const
    {
        GLint location = GetUniform(name);
        if (location != -1)
        {
            glUniform3iv(location, 1, glm::value_ptr(value));
        }
    }

    void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
    {
        GLint location = GetUniform(name);
        if (location != -1)
        {
            glUniform4fv(location, 1, glm::value_ptr(value));
        }
    }

    void Shader::SetMat3(const std::string& name, const glm::mat3& mat) const
    {
        GLint location = GetUniform(name);
        if (location != -1)
        {
            glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
        }
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
    {
        GLint location = GetUniform(name);
        if (location != -1)
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
        }
    }
}