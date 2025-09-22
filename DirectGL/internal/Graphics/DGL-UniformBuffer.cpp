module;

#include <glad/gl.h>

module DGL;

namespace DGL
{
	UniformBuffer::UniformBuffer(const void* data, const size_t sizeInBytes)
	{
		glCreateBuffers(1, &m_BufferId);
		glNamedBufferStorage(m_BufferId, sizeInBytes, data, GL_DYNAMIC_STORAGE_BIT);
	}

	void UniformBuffer::Bind()
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_BufferId);
	}

	void UniformBuffer::Upload(const void* data, const size_t sizeInBytes)
	{
		glNamedBufferSubData(m_BufferId, 0, sizeInBytes, data);
	}
}