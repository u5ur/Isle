// VertexArray.cpp
#include "VertexArray.h"

namespace Isle
{
	IndexBuffer::~IndexBuffer()
	{
		if (m_Id)
			glDeleteBuffers(1, &m_Id);
	}

	void IndexBuffer::Create(const std::vector<unsigned int>& data)
	{
		m_Data = data;

		glGenBuffers(1, &m_Id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Data.size() * sizeof(unsigned int), m_Data.data(), GL_STATIC_DRAW);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
	}

	template <typename T>
	VertexBuffer<T>::~VertexBuffer()
	{
		if (m_Id)
			glDeleteBuffers(1, &m_Id);
	}

	template <typename T>
	void VertexBuffer<T>::Create(const std::vector<T>& data)
	{
		m_Data = data;

		glGenBuffers(1, &m_Id);
		glBindBuffer(GL_ARRAY_BUFFER, m_Id);
		glBufferData(GL_ARRAY_BUFFER, m_Data.size() * sizeof(T), m_Data.data(), GL_STATIC_DRAW);
	}

	template <typename T>
	void VertexBuffer<T>::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Id);
	}

	template <typename T>
	VertexArray<T>::VertexArray()
	{
		glGenVertexArrays(1, &m_Id);
	}

	template <typename T>
	VertexArray<T>::VertexArray(VertexBuffer<T>* vertex_buffer, IndexBuffer* index_buffer)
	{
		glGenVertexArrays(1, &m_Id);
		Create(vertex_buffer, index_buffer);
	}

	template <typename T>
	VertexArray<T>::VertexArray(std::vector<T>& vertex_buffer, std::vector<unsigned int>& index_buffer)
	{
		glGenVertexArrays(1, &m_Id);
		m_VertexBuffer = new VertexBuffer<T>();
		m_VertexBuffer->Create(vertex_buffer);

		m_IndexBuffer = new IndexBuffer();
		m_IndexBuffer->Create(index_buffer);

		Create(m_VertexBuffer, m_IndexBuffer);
	}

	template <typename T>
	VertexArray<T>::~VertexArray()
	{
		if (m_Id)
			glDeleteVertexArrays(1, &m_Id);
	}

	template <typename T>
	void VertexArray<T>::Bind() const
	{
		glBindVertexArray(m_Id);
	}

	template <typename T>
	void VertexArray<T>::Create(VertexBuffer<T>* vertex_buffer, IndexBuffer* index_buffer)
	{
		m_VertexBuffer = vertex_buffer;
		m_IndexBuffer = index_buffer;

		glBindVertexArray(m_Id);
		vertex_buffer->Bind();
		index_buffer->Bind();

		SetupAttributes();

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	template <typename T>
	void VertexArray<T>::Draw() const
	{
		if (!m_IndexBuffer)
			return;

		glBindVertexArray(m_Id);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_IndexBuffer->m_Data.size()), GL_UNSIGNED_INT, 0);
	}

	template <>
	void VertexArray<Vertex>::SetupAttributes()
	{
		GLsizei stride = sizeof(Vertex);
		size_t offset = 0;

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += sizeof(glm::vec3);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += sizeof(glm::vec3);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += sizeof(glm::vec2);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += sizeof(glm::vec3);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += sizeof(glm::vec3);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	}

	template <>
	void VertexArray<SkinnedVertex>::SetupAttributes()
	{
		GLsizei stride = sizeof(SkinnedVertex);
		size_t offset = 0;

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += sizeof(glm::vec3);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += sizeof(glm::vec3);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += sizeof(glm::vec2);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += sizeof(glm::vec3);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += sizeof(glm::vec3);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += sizeof(glm::vec4);

		glEnableVertexAttribArray(6);
		glVertexAttribIPointer(6, 4, GL_INT, stride, (void*)offset);
		offset += sizeof(glm::ivec4);

		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	}

	template class Isle::VertexBuffer<Isle::Vertex>;
	template class Isle::VertexBuffer<Isle::SkinnedVertex>;
	template class Isle::VertexArray<Isle::Vertex>;
	template class Isle::VertexArray<Isle::SkinnedVertex>;
}
