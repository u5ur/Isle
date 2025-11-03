// VertexArray.h
#pragma once
#include <Core/Common/Common.h>

namespace Isle
{
	struct Vertex
	{
		glm::vec3 m_Position;
		glm::vec3 m_Normal;
		glm::vec2 m_TexCoord;
		glm::vec3 m_Tangent;
		glm::vec3 m_BitTangent;
		glm::vec4 m_Color;
	};

	struct SkinnedVertex
	{
		glm::vec3 m_Position;
		glm::vec3 m_Normal;
		glm::vec2 m_TexCoord;
		glm::vec3 m_Tangent;
		glm::vec3 m_BitTangent;
		glm::vec4 m_Color;
		glm::ivec4 m_BoneIds;
		glm::vec4 m_BoneWeights;
	};

	class IVertexArray
	{
	public:
		virtual ~IVertexArray() = default;
		virtual void Bind() const = 0;
		virtual void Draw() const = 0;
	};

	template <typename T>
	class VertexBuffer
	{
	public:
		GLuint m_Id = 0;
		std::vector<T> m_Data;

	public:
		VertexBuffer() = default;
		~VertexBuffer();

		void Create(const std::vector<T>& data);
		void Bind() const;
	};

	class IndexBuffer
	{
	public:
		GLuint m_Id = 0;
		std::vector<unsigned int> m_Data;

	public:
		IndexBuffer() = default;
		~IndexBuffer();

		void Create(const std::vector<unsigned int>& data);
		void Bind() const;
	};

	template <typename T>
	class VertexArray : public IVertexArray
	{
	public:
		GLuint m_Id = 0;
		VertexBuffer<T>* m_VertexBuffer = nullptr;
		IndexBuffer* m_IndexBuffer = nullptr;

	public:
		VertexArray();
		VertexArray(VertexBuffer<T>* vertex_buffer, IndexBuffer* index_buffer);
		VertexArray(std::vector<T>& vertex_buffer, std::vector<unsigned int>& index_buffer);
		~VertexArray();

		void Bind() const override;
		void Draw() const override;
		void Create(VertexBuffer<T>* vertex_buffer, IndexBuffer* index_buffer);

	private:
		void SetupAttributes();
	};
}