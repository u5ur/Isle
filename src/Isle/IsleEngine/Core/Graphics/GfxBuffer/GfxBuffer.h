#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/GfxResource/GfxResource.h>
#include <unordered_map>

namespace Isle
{
    enum class GFX_BUFFER_TYPE
    {
        UNKNOWN,
        VERTEX,
        INDEX,
        UNIFORM,
        STORAGE,
        ATOMIC_COUNTER,
        INDIRECT_DRAW,
        INDIRECT_DISPATCH,
        PIXEL_PACK,
        PIXEL_UNPACK,
        TRANSFORM_FEEDBACK,
    };

    enum class GFX_BUFFER_USAGE
    {
        STATIC,
        DYNAMIC,
        STREAM
    };

    struct VertexAttribute
    {
        GLuint index;
        GLint size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        const void* pointer;
        GLuint divisor = 0;
    };

    class GfxBuffer : public GfxResource
    {
    private:
        GLuint m_Id = 0;
        GLuint m_VAO = 0;
        GFX_BUFFER_TYPE m_Type = GFX_BUFFER_TYPE::UNKNOWN;
        GFX_BUFFER_USAGE m_Usage = GFX_BUFFER_USAGE::DYNAMIC;
        GLenum m_Target = 0;
        GLenum m_UsageHint = GL_STATIC_DRAW;

        bool m_Mapped = false;
        bool m_Dirty = false;

        std::vector<uint8_t> m_LocalData;
        std::vector<VertexAttribute> m_VertexAttributes;

        GfxBuffer* m_AssociatedIndexBuffer = nullptr;

    public:
        GfxBuffer() = default;
        ~GfxBuffer() override;

        GfxBuffer(GFX_BUFFER_TYPE type, GLsizeiptr size, const void* data = nullptr,
            GFX_BUFFER_USAGE usage = GFX_BUFFER_USAGE::STATIC)
        {
            Create(type, size, data, usage);
        }

        void Create(GFX_BUFFER_TYPE type, GLsizeiptr size, const void* data = nullptr,
            GFX_BUFFER_USAGE usage = GFX_BUFFER_USAGE::STATIC);
        void Destroy();

        void Load() override { Upload(); }
        void Unload() override { Destroy(); }

        void Bind(uint32_t slot = 0) override;
        void Unbind(uint32_t slot = 0) override;

        void AddVertexAttribute(GLuint index, GLint size, GLenum type,
            GLboolean normalized, GLsizei stride,
            const void* pointer, GLuint divisor = 0);
        void SetupVertexAttributes();
        void SetIndexBuffer(GfxBuffer* indexBuffer);

        void Upload();
        void Download();
        void Clear(GLenum internalFormat = GL_R32UI,
            GLenum format = GL_RED_INTEGER,
            GLenum type = GL_UNSIGNED_INT,
            const void* clearValue = nullptr);

        void* Map(GLenum access = GL_READ_WRITE);
        void  Unmap();

        template<typename T>
        void SetData(const std::vector<T>& data)
        {
            m_LocalData.resize(data.size() * sizeof(T));
            std::memcpy(m_LocalData.data(), data.data(), m_LocalData.size());
            m_Dirty = true;
        }

        template<typename T>
        std::vector<T> GetData() const
        {
            std::vector<T> out(m_LocalData.size() / sizeof(T));
            std::memcpy(out.data(), m_LocalData.data(), m_LocalData.size());
            return out;
        }

        template<typename T>
        size_t Add(const T& element)
        {
            if (m_Type != GFX_BUFFER_TYPE::STORAGE)
            {
                ISLE_WARN("GfxBuffer::Add() called on non-storage buffer");
                return 0;
            }

            size_t offset = m_LocalData.size();
            size_t bytes = sizeof(T);

            const uint8_t* src = reinterpret_cast<const uint8_t*>(&element);
            m_LocalData.insert(m_LocalData.end(), src, src + bytes);

            if (m_Id)
            {
                glBindBuffer(m_Target, m_Id);
                glBufferData(m_Target, m_LocalData.size(), m_LocalData.data(), m_UsageHint);
                glBindBuffer(m_Target, 0);
            }

            m_Dirty = false;
            m_SizeInBytes = m_LocalData.size();
            return offset;
        }

        template<typename T>
        size_t AddRange(const std::vector<T>& elements)
        {
            if (m_Type != GFX_BUFFER_TYPE::STORAGE)
            {
                ISLE_WARN("GfxBuffer::AddRange() called on non-storage buffer");
                return 0;
            }

            size_t offset = m_LocalData.size();
            size_t bytes = elements.size() * sizeof(T);

            const uint8_t* src = reinterpret_cast<const uint8_t*>(elements.data());
            m_LocalData.insert(m_LocalData.end(), src, src + bytes);

            if (m_Id)
            {
                glBindBuffer(m_Target, m_Id);
                glBufferData(m_Target, m_LocalData.size(), m_LocalData.data(), m_UsageHint);
                glBindBuffer(m_Target, 0);
            }

            m_Dirty = false;
            m_SizeInBytes = m_LocalData.size();
            return offset;
        }


        void SetDebugLabel(const std::string& name);
        static GLenum ResolveTarget(GFX_BUFFER_TYPE type);
        static GLenum ResolveUsage(GFX_BUFFER_USAGE usage);

        bool IsDirty() const { return m_Dirty; }
        GLsizeiptr GetSize() const { return static_cast<GLsizeiptr>(m_LocalData.size()); }
        GLuint GetId() const { return m_Id; }
        GLuint GetVAO() const { return m_VAO; }
        GFX_BUFFER_TYPE GetType() const { return m_Type; }
    };
}
