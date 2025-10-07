module;

#include <glad/gl.h>
#include <format>

module DirectGL.RendererOpenGL;

import Preconditions;

namespace DGL::RendererOpenGL
{
	constexpr GLint WrapModeToGlId(const Renderer::TextureWrapModeId id)
	{
		switch (id)
		{
			using enum Renderer::TextureWrapModeId;
			case Repeat: return GL_REPEAT;
			case MirroredRepeat: return GL_MIRRORED_REPEAT;
			case ClampToEdge: return GL_CLAMP_TO_EDGE;
			case ClampToBorder: return GL_CLAMP_TO_BORDER;
			default: System::Error(std::format("Unsupported TextureWrapModeId {}", id));
		}
	}

	constexpr GLint FilterModeToGlId(const Renderer::TextureFilterModeId id)
	{
		switch (id)
		{
			using enum Renderer::TextureFilterModeId;
			case Nearest: return GL_NEAREST;
			case Linear: return GL_LINEAR;
			default: System::Error(std::format("Unsupported TextureFilterModeId {}", id));
		}
	}

	std::unique_ptr<OpenGLTextureSampler> OpenGLTextureSampler::Create(const Renderer::TextureWrapMode wrapMode, const Renderer::TextureFilterMode filterMode)
	{
		GLuint samplerId = 0;
		glCreateSamplers(1, &samplerId);
		if (samplerId == 0)
		{
			s_Logger.Log(LogLevel::Error, "Failed to create texture sampler");
			return nullptr;
		}

		// Apply initial parameters immediately
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, WrapModeToGlId(wrapMode.Horizontal));
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, WrapModeToGlId(wrapMode.Vertical));
		glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, FilterModeToGlId(filterMode.Minification));
		glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, FilterModeToGlId(filterMode.Magnification));

		return std::unique_ptr<OpenGLTextureSampler>(new OpenGLTextureSampler(samplerId, wrapMode, filterMode));
	}

	OpenGLTextureSampler::~OpenGLTextureSampler()
	{
		if (m_RendererId != 0)
		{
			glDeleteSamplers(1, &m_RendererId);
		}
	}

	void OpenGLTextureSampler::SetWrapMode(const Renderer::TextureWrapMode mode)
	{
		m_WrapMode = mode;

		glSamplerParameteri(m_RendererId, GL_TEXTURE_WRAP_S, WrapModeToGlId(mode.Horizontal));
		glSamplerParameteri(m_RendererId, GL_TEXTURE_WRAP_T, WrapModeToGlId(mode.Vertical));
	}

	Renderer::TextureWrapMode OpenGLTextureSampler::GetWrapMode() const
	{
		return m_WrapMode;
	}

	void OpenGLTextureSampler::SetFilterMode(const Renderer::TextureFilterMode mode)
	{
		m_FilterMode = mode;

		glSamplerParameteri(m_RendererId, GL_TEXTURE_MIN_FILTER, FilterModeToGlId(mode.Minification));
		glSamplerParameteri(m_RendererId, GL_TEXTURE_MAG_FILTER, FilterModeToGlId(mode.Magnification));
	}

	Renderer::TextureFilterMode OpenGLTextureSampler::GetFilterMode() const
	{
		return m_FilterMode;
	}

	GLuint OpenGLTextureSampler::GetRendererId() const
	{
		return m_RendererId;
	}

	OpenGLTextureSampler::OpenGLTextureSampler(const GLuint id, const Renderer::TextureWrapMode wrapMode, const Renderer::TextureFilterMode filterMode):
		m_RendererId(id),
		m_WrapMode(wrapMode),
		m_FilterMode(filterMode)
	{
	}
}