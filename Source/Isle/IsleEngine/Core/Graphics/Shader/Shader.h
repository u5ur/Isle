// Shader.h
#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/GfxResource/GfxResource.h>

namespace Isle
{
    enum class SHADER_TYPE : uint8_t
    {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        COMPUTE,
    };

    class Shader : public Object
    {
    public:
        std::string m_Path;
        GLuint m_ProgramId = 0;

    private:
        std::vector<GLuint> m_AttachedShaders;

    public:
        Shader();
        ~Shader();

        void Bind() const;
        bool LoadFromFile(SHADER_TYPE type, const std::string& path);
        bool LoadFromSource(SHADER_TYPE type, const std::string& source);
        bool Link();

        void DispatchCompute(GLuint groupsX, GLuint groupsY, GLuint groupsZ) const;

        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetUInt(const std::string& name, unsigned int value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetVec2(const std::string& name, const glm::vec2& value) const;
        void SetVec3(const std::string& name, const glm::vec3& value) const;
        void SetVec4(const std::string& name, const glm::vec4& value) const;
        void SetIVec2(const std::string& name, const glm::ivec2& value) const;
        void SetIVec3(const std::string& name, const glm::ivec3& value) const;
        void SetMat3(const std::string& name, const glm::mat3& mat) const;
        void SetMat4(const std::string& name, const glm::mat4& mat) const;

    private:
        static GLenum ResolveShaderType(SHADER_TYPE type);
        GLuint CompileShader(SHADER_TYPE type, const std::string& source);
        bool CheckShaderErrors(GLuint shader, const std::string& type);
        bool CheckProgramErrors(GLuint program);
        std::string ProcessIncludes(std::string& source, const std::string& base_path);
        GLint GetUniform(const std::string& name) const;
    };
}