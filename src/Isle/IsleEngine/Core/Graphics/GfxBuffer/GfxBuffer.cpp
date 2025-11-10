#include "GfxBuffer.h"

namespace Isle
{
    GfxBuffer::~GfxBuffer()
    {
        Destroy();
    }

    void GfxBuffer::Create(GFX_BUFFER_TYPE type, GLsizeiptr size, const void* data, GFX_BUFFER_USAGE usage)
    {
        m_Type = type;
        m_Usage = usage;
        m_Target = ResolveTarget(type);
        m_UsageHint = ResolveUsage(usage);

        if (type == GFX_BUFFER_TYPE::VERTEX && !m_VAO)
        {
            glGenVertexArrays(1, &m_VAO);
        }

        if (!m_Id)
            glGenBuffers(1, &m_Id);

        if (size > 0)
        {
            m_LocalData.resize(size);
            if (data)
                std::memcpy(m_LocalData.data(), data, size);

            glBindBuffer(m_Target, m_Id);
            glBufferData(m_Target, size, m_LocalData.data(), m_UsageHint);
            glBindBuffer(m_Target, 0);
            m_SizeInBytes = static_cast<size_t>(size);
        }

        m_IsLoaded = true;
        m_Dirty = false;
    }

    void GfxBuffer::Destroy()
    {
        if (m_VAO)
        {
            glDeleteVertexArrays(1, &m_VAO);
            m_VAO = 0;
        }

        if (m_Id)
        {
            glDeleteBuffers(1, &m_Id);
            m_Id = 0;
        }

        m_LocalData.clear();
        m_VertexAttributes.clear();
        m_AssociatedIndexBuffer = nullptr;
        m_IsLoaded = false;
        m_IsResident = false;
        m_SizeInBytes = 0;
    }

    void GfxBuffer::Bind(uint32_t slot)
    {
        if (!m_Id) return;

        switch (m_Type)
        {
        case GFX_BUFFER_TYPE::VERTEX:
            if (m_VAO)
            {
                glBindVertexArray(m_VAO);
                m_IsResident = true;
            }
            break;

        case GFX_BUFFER_TYPE::INDEX:
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
            m_IsResident = true;
            break;

        case GFX_BUFFER_TYPE::UNIFORM:
            glBindBufferBase(GL_UNIFORM_BUFFER, slot, m_Id);
            m_IsResident = true;
            break;

        case GFX_BUFFER_TYPE::STORAGE:
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, m_Id);
            m_IsResident = true;
            break;

        case GFX_BUFFER_TYPE::ATOMIC_COUNTER:
            glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, slot, m_Id);
            m_IsResident = true;
            break;

        case GFX_BUFFER_TYPE::TRANSFORM_FEEDBACK:
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, slot, m_Id);
            m_IsResident = true;
            break;

        case GFX_BUFFER_TYPE::INDIRECT_DRAW:
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_Id);
            m_IsResident = true;
            break;

        case GFX_BUFFER_TYPE::INDIRECT_DISPATCH:
            glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, m_Id);
            m_IsResident = true;
            break;

        case GFX_BUFFER_TYPE::PIXEL_PACK:
            glBindBuffer(GL_PIXEL_PACK_BUFFER, m_Id);
            m_IsResident = true;
            break;

        case GFX_BUFFER_TYPE::PIXEL_UNPACK:
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_Id);
            m_IsResident = true;
            break;

        default:
            glBindBuffer(m_Target, m_Id);
            m_IsResident = true;
            break;
        }
    }

    void GfxBuffer::Unbind(uint32_t)
    {
        switch (m_Type)
        {
        case GFX_BUFFER_TYPE::VERTEX:
            if (m_VAO)
                glBindVertexArray(0);
            break;

        case GFX_BUFFER_TYPE::INDEX:
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            break;

        case GFX_BUFFER_TYPE::UNIFORM:
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            break;

        case GFX_BUFFER_TYPE::STORAGE:
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            break;

        case GFX_BUFFER_TYPE::ATOMIC_COUNTER:
            glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
            break;

        case GFX_BUFFER_TYPE::TRANSFORM_FEEDBACK:
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
            break;

        default:
            glBindBuffer(m_Target, 0);
            break;
        }
        m_IsResident = false;
    }

    void GfxBuffer::AddVertexAttribute(GLuint index, GLint size, GLenum type,
        GLboolean normalized, GLsizei stride,
        const void* pointer, GLuint divisor)
    {
        if (m_Type != GFX_BUFFER_TYPE::VERTEX)
        {
            return;
        }

        VertexAttribute attrib;
        attrib.index = index;
        attrib.size = size;
        attrib.type = type;
        attrib.normalized = normalized;
        attrib.stride = stride;
        attrib.pointer = pointer;
        attrib.divisor = divisor;

        m_VertexAttributes.push_back(attrib);
    }

    void GfxBuffer::SetupVertexAttributes()
    {
        if (m_Type != GFX_BUFFER_TYPE::VERTEX || !m_VAO)
            return;

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_Id);

        for (const auto& attrib : m_VertexAttributes)
        {
            glEnableVertexAttribArray(attrib.index);
            glVertexAttribPointer(attrib.index, attrib.size, attrib.type,
                attrib.normalized, attrib.stride, attrib.pointer);

            if (attrib.divisor > 0)
            {
                glVertexAttribDivisor(attrib.index, attrib.divisor);
            }
        }

        if (m_AssociatedIndexBuffer && m_AssociatedIndexBuffer->GetType() == GFX_BUFFER_TYPE::INDEX)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_AssociatedIndexBuffer->GetId());
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void GfxBuffer::SetIndexBuffer(GfxBuffer* indexBuffer)
    {
        if (m_Type != GFX_BUFFER_TYPE::VERTEX || !m_VAO)
            return;

        m_AssociatedIndexBuffer = indexBuffer;

        if (m_AssociatedIndexBuffer)
        {
            glBindVertexArray(m_VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_AssociatedIndexBuffer->GetId());
            glBindVertexArray(0);
        }
    }

    void GfxBuffer::Upload()
    {
        if (!m_Id || m_LocalData.empty() || !m_Dirty)
            return;

        glBindBuffer(m_Target, m_Id);

        GLint currentSize = 0;
        glGetBufferParameteriv(m_Target, GL_BUFFER_SIZE, &currentSize);

        if (currentSize == 0 || m_LocalData.size() > static_cast<size_t>(currentSize))
        {
            glBufferData(m_Target, m_LocalData.size(), m_LocalData.data(), m_UsageHint);
        }
        else
        {
            glBufferSubData(m_Target, 0, m_LocalData.size(), m_LocalData.data());
        }

        glBindBuffer(m_Target, 0);
        m_Dirty = false;
    }

    void GfxBuffer::Download()
    {
        if (!m_Id || m_LocalData.empty())
            return;

        glBindBuffer(m_Target, m_Id);
        glGetBufferSubData(m_Target, 0, m_LocalData.size(), m_LocalData.data());
        glBindBuffer(m_Target, 0);
    }

    void GfxBuffer::Clear(GLenum internalFormat, GLenum format, GLenum type, const void* clearValue)
    {
        glBindBuffer(m_Target, m_Id);
        glClearBufferData(m_Target, internalFormat, format, type, clearValue);
        glBindBuffer(m_Target, 0);
        std::fill(m_LocalData.begin(), m_LocalData.end(), 0);
    }

    void* GfxBuffer::Map(GLenum access)
    {
        if (m_Mapped) return nullptr;
        glBindBuffer(m_Target, m_Id);
        void* ptr = glMapBuffer(m_Target, access);
        glBindBuffer(m_Target, 0);
        m_Mapped = true;
        return ptr;
    }

    void GfxBuffer::Unmap()
    {
        if (!m_Mapped) return;
        glBindBuffer(m_Target, m_Id);
        glUnmapBuffer(m_Target);
        glBindBuffer(m_Target, 0);
        m_Mapped = false;
    }

    void GfxBuffer::SetDebugLabel(const std::string& name)
    {
        if (glObjectLabel)
        {
            glObjectLabel(GL_BUFFER, m_Id, -1, name.c_str());
            if (m_VAO)
                glObjectLabel(GL_VERTEX_ARRAY, m_VAO, -1, (name + "_VAO").c_str());
        }
    }

    GLenum GfxBuffer::ResolveTarget(GFX_BUFFER_TYPE type)
    {
        switch (type)
        {
        case GFX_BUFFER_TYPE::VERTEX:             return GL_ARRAY_BUFFER;
        case GFX_BUFFER_TYPE::INDEX:              return GL_ELEMENT_ARRAY_BUFFER;
        case GFX_BUFFER_TYPE::UNIFORM:            return GL_UNIFORM_BUFFER;
        case GFX_BUFFER_TYPE::STORAGE:            return GL_SHADER_STORAGE_BUFFER;
        case GFX_BUFFER_TYPE::ATOMIC_COUNTER:     return GL_ATOMIC_COUNTER_BUFFER;
        case GFX_BUFFER_TYPE::INDIRECT_DRAW:      return GL_DRAW_INDIRECT_BUFFER;
        case GFX_BUFFER_TYPE::INDIRECT_DISPATCH:  return GL_DISPATCH_INDIRECT_BUFFER;
        case GFX_BUFFER_TYPE::PIXEL_PACK:         return GL_PIXEL_PACK_BUFFER;
        case GFX_BUFFER_TYPE::PIXEL_UNPACK:       return GL_PIXEL_UNPACK_BUFFER;
        case GFX_BUFFER_TYPE::TRANSFORM_FEEDBACK: return GL_TRANSFORM_FEEDBACK_BUFFER;
        default:                                  return GL_ARRAY_BUFFER;
        }
    }

    GLenum GfxBuffer::ResolveUsage(GFX_BUFFER_USAGE usage)
    {
        switch (usage)
        {
        case GFX_BUFFER_USAGE::DYNAMIC: return GL_DYNAMIC_DRAW;
        case GFX_BUFFER_USAGE::STREAM:  return GL_STREAM_DRAW;
        default:                        return GL_STATIC_DRAW;
        }
    }
}