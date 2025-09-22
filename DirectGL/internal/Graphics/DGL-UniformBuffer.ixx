// Project Name : DirectGL
// File Name    : DGL-UniformBuffer.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <glad/gl.h>

export module DGL:UniformBuffer;

namespace DGL
{
	class UniformBuffer
	{
	public:

		explicit UniformBuffer(const void* data, size_t sizeInBytes);

		void Bind();
		void Upload(const void* data, size_t sizeInBytes);

	private:

		GLuint m_BufferId;

	};
}