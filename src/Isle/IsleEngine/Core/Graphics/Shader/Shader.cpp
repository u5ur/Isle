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
        for (GLuint shader : m_AttachedShaders)
            glDeleteShader(shader);

        if (m_ProgramId)
            glDeleteProgram(m_ProgramId);
    }

    GLenum Shader::ResolveShaderType(SHADER_TYPE type)
    {
        switch (type)
        {
        case SHADER_TYPE::VERTEX:   return GL_VERTEX_SHADER;
        case SHADER_TYPE::FRAGMENT: return GL_FRAGMENT_SHADER;
        case SHADER_TYPE::GEOMETRY: return GL_GEOMETRY_SHADER;
        case SHADER_TYPE::COMPUTE:  return GL_COMPUTE_SHADER;
        default:                     return GL_VERTEX_SHADER;
        }
    }

    bool Shader::CheckShaderErrors(GLuint shader, const std::string& type)
    {
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[1024];
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            ISLE_ERROR("SHADER_COMPILATION_ERROR (%s): %s\n", type.c_str(), infoLog);
            return false;
        }
        return true;
    }

    bool Shader::CheckProgramErrors(GLuint program)
    {
        GLint success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[1024];
            glGetProgramInfoLog(program, 1024, nullptr, infoLog);
            ISLE_ERROR("PROGRAM_LINKING_ERROR: %s\n", infoLog);
            return false;
        }
        return true;
    }

    GLuint Shader::CompileShader(SHADER_TYPE type, const std::string& source)
    {
        GLenum glType = ResolveShaderType(type);
        GLuint shader = glCreateShader(glType);

        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        std::string label;
        switch (type)
        {
        case SHADER_TYPE::VERTEX:   label = "VERTEX"; break;
        case SHADER_TYPE::FRAGMENT: label = "FRAGMENT"; break;
        case SHADER_TYPE::GEOMETRY: label = "GEOMETRY"; break;
        case SHADER_TYPE::COMPUTE:  label = "COMPUTE"; break;
        }

        if (!CheckShaderErrors(shader, label))
        {
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    bool Shader::LoadFromSource(SHADER_TYPE type, const std::string& source)
    {
        GLuint shader = CompileShader(type, source);
        if (!shader)
            return false;

        glAttachShader(m_ProgramId, shader);
        m_AttachedShaders.push_back(shader);
        return true;
    }

    bool Shader::LoadFromFile(SHADER_TYPE type, const std::string& path)
    {
        std::string source;
        READ_FILE(path, source);
        if (source.empty())
        {
            ISLE_ERROR("Failed to read shader: %s\n", path.c_str());
            return false;
        }

        source = ProcessIncludes(source, path);
        return LoadFromSource(type, source);
    }

    bool Shader::Link()
    {
        glLinkProgram(m_ProgramId);
        if (!CheckProgramErrors(m_ProgramId))
            return false;

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


        for (GLuint shader : m_AttachedShaders)
            glDetachShader(m_ProgramId, shader);

        m_AttachedShaders.clear();
        return true;
    }

    void Shader::Bind() const
    {
        glUseProgram(m_ProgramId);
    }

    void Shader::DispatchCompute(GLuint groupsX, GLuint groupsY, GLuint groupsZ) const
    {
        glDispatchCompute(groupsX, groupsY, groupsZ);
    }

    std::string Shader::ProcessIncludes(std::string& source, const std::string& basePath)
    {
        std::ostringstream output;
        std::istringstream input(source);
        std::string line;

        static thread_local std::unordered_set<std::string> visitedIncludes;

        size_t lastSlash = basePath.find_last_of("/\\");
        std::string baseDir = (lastSlash != std::string::npos) ? basePath.substr(0, lastSlash + 1) : "";

        auto normalizePath = [](std::string path) -> std::string {
            std::replace(path.begin(), path.end(), '\\', '/');
            return path;
            };

        while (std::getline(input, line))
        {
            std::string trimmed = line;
            trimmed.erase(0, trimmed.find_first_not_of(" \t"));
            if (trimmed.rfind("#include", 0) == 0)
            {
                size_t firstQuote = line.find_first_of("\"<");
                size_t secondQuote = line.find_last_of("\">");
                if (firstQuote == std::string::npos || secondQuote == std::string::npos || secondQuote <= firstQuote)
                {
                    ISLE_WARN("Malformed include directive in shader: %s\n", line.c_str());
                    continue;
                }

                std::string includeFile = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
                std::string fullPath = normalizePath(baseDir + includeFile);

                if (visitedIncludes.find(fullPath) != visitedIncludes.end())
                {
                    ISLE_WARN("Skipping duplicate include: %s\n", fullPath.c_str());
                    continue;
                }

                visitedIncludes.insert(fullPath);

                std::string includeContent;
                READ_FILE(fullPath, includeContent);

                if (!includeContent.empty())
                {
                    includeContent = ProcessIncludes(includeContent, fullPath);
                    output << "// BEGIN INCLUDE: " << includeFile << "\n";
                    output << includeContent << "\n";
                    output << "// END INCLUDE: " << includeFile << "\n";
                }
                else
                {
                    ISLE_WARN("Failed to include file: %s\n", fullPath.c_str());
                }
            }
            else
            {
                output << line << "\n";
            }
        }

        if (basePath == normalizePath(basePath))
            visitedIncludes.clear();

        return output.str();
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
