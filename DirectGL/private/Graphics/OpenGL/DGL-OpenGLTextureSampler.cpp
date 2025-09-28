module;

#include <glad/gl.h>

module DGL;

namespace DGL
{
	std::unique_ptr<OpenGLTextureSampler> OpenGLTextureSampler::Create()
	{
		GLuint samplerId = 0;
		glCreateSamplers(1, &samplerId);
		glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, GL_REPEAT);

		return std::unique_ptr<OpenGLTextureSampler>(new OpenGLTextureSampler(samplerId));
	}

	OpenGLTextureSampler::~OpenGLTextureSampler()
	{
		if (m_SamplerId != 0)
		{
			glDeleteSamplers(1, &m_SamplerId);
		}
	}

	void OpenGLTextureSampler::Bind(const uint32_t slot) const
	{
		glBindSampler(slot, m_SamplerId);
	}

	OpenGLTextureSampler::OpenGLTextureSampler(const GLuint id):
		m_SamplerId(id)
	{
	}
}