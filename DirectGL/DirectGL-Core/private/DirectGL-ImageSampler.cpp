module;

#include <glad/gl.h>

module DirectGL;

import DirectGL.ControlFlow;

import :ImageSampler;

namespace DGL
{
	constexpr GLenum ImageSamplerFilterModeTypeToGLFilter(const ImageSamplerFilterModeType filterMode)
	{
		switch (filterMode)
		{
			case ImageSamplerFilterModeType::Nearest:	return GL_NEAREST;
			case ImageSamplerFilterModeType::Linear:	return GL_LINEAR;
			default:									ThrowError("Unknown ImageSamplerFilterModeType in ImageSamplerFilterModeTypeToGLMinFilter()");
		}
	}

	constexpr GLenum ImageSamplerWrapModeTypeToGLWrap(const ImageSamplerWrapModeType wrapMode)
	{
		switch (wrapMode)
		{
			case ImageSamplerWrapModeType::Repeat:			return GL_REPEAT;
			case ImageSamplerWrapModeType::MirroredRepeat:	return GL_MIRRORED_REPEAT;
			case ImageSamplerWrapModeType::ClampToEdge:		return GL_CLAMP_TO_EDGE;
			case ImageSamplerWrapModeType::ClampToBorder:	return GL_CLAMP_TO_BORDER;
			default:										ThrowError("Unknown ImageSamplerWrapModeType in ImageSamplerWrapModeTypeToGLWrap()");
		}
	}

	std::unique_ptr<ImageSampler> ImageSampler::Create(const ImageSamplerFilterMode filterMode, const ImageSamplerWrapMode wrapMode)
	{
		GLuint samplerId = 0;
		glCreateSamplers(1, &samplerId);
		glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, ImageSamplerFilterModeTypeToGLFilter(filterMode.MinFilter));
		glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, ImageSamplerFilterModeTypeToGLFilter(filterMode.MagFilter));
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, ImageSamplerWrapModeTypeToGLWrap(wrapMode.WrapS));
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, ImageSamplerWrapModeTypeToGLWrap(wrapMode.WrapT));

		return std::unique_ptr<ImageSampler>(new ImageSampler(samplerId, filterMode, wrapMode));
	}

	ImageSampler::~ImageSampler()
	{
		glDeleteSamplers(1, &m_SamplerId);
	}

	void ImageSampler::SetFilterMode(const ImageSamplerFilterMode filterMode)
	{
		if (m_FilterMode == filterMode)
		{
			return;
		}

		glSamplerParameteri(m_SamplerId, GL_TEXTURE_MIN_FILTER, ImageSamplerFilterModeTypeToGLFilter(filterMode.MinFilter));
		glSamplerParameteri(m_SamplerId, GL_TEXTURE_MAG_FILTER, ImageSamplerFilterModeTypeToGLFilter(filterMode.MagFilter));
		m_FilterMode = filterMode;
	}

	ImageSamplerFilterMode ImageSampler::GetFilterMode() const
	{
		return m_FilterMode;
	}

	void ImageSampler::SetWrapMode(const ImageSamplerWrapMode wrapMode)
	{
		if (m_WrapMode == wrapMode)
		{
			return;
		}

		glSamplerParameteri(m_SamplerId, GL_TEXTURE_WRAP_S, ImageSamplerWrapModeTypeToGLWrap(wrapMode.WrapS));
		glSamplerParameteri(m_SamplerId, GL_TEXTURE_WRAP_T, ImageSamplerWrapModeTypeToGLWrap(wrapMode.WrapT));
		m_WrapMode = wrapMode;
	}

	ImageSamplerWrapMode ImageSampler::GetWrapMode() const
	{
		return m_WrapMode;
	}

	uint32_t ImageSampler::GetSamplerId() const
	{
		return m_SamplerId;
	}

	ImageSampler::ImageSampler(const uint32_t samplerId, const ImageSamplerFilterMode filterMode, const ImageSamplerWrapMode wrapMode):
		m_SamplerId(samplerId),
		m_FilterMode(filterMode),
		m_WrapMode(wrapMode)
	{
	}
}