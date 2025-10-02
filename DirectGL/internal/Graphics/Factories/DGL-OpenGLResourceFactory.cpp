module;

#include <memory>
#include <cstdint>
#include <string_view>

module DGL;

import :OpenGLRenderer;
import :OpenGLShader;

namespace DGL
{
	std::unique_ptr<WindowRenderTarget> OpenGLResourceFactory::CreateWindowRenderTarget(System::Window& window, Renderer& renderer)
	{
		return OpenGLWindowRenderTarget::Create(window, renderer);
	}

	std::unique_ptr<OffscreenRenderTarget> OpenGLResourceFactory::CreateFramebuffer(const uint32_t width, const uint32_t height, Renderer& renderer)
	{
		return OpenGLOffscreenRenderTarget::Create(width, height, renderer);
	}

	std::unique_ptr<Renderer> OpenGLResourceFactory::CreateRenderer(const uint32_t maxVertices)
	{
		return OpenGLRenderer::Create(maxVertices);
	}

	std::unique_ptr<Shader> OpenGLResourceFactory::CreateShader(const std::string_view source, const ShaderType type)
	{
		return OpenGLShader::Create(source, type);
	}

	std::unique_ptr<ShaderProgram> OpenGLResourceFactory::CreateShaderProgram(const Shader& vertexShader, const Shader& fragmentShader)
	{
		return OpenGLShaderProgram::Create(vertexShader, fragmentShader);
	}

	std::unique_ptr<Texture> OpenGLResourceFactory::CreateTexture(const std::filesystem::path& filepath)
	{
		return OpenGLTexture::Create(filepath);
	}

	std::unique_ptr<TextureSampler> OpenGLResourceFactory::CreateTextureSampler(const TextureWrapMode wrapMode, const TextureFilterMode filterMode)
	{
		return OpenGLTextureSampler::Create(wrapMode, filterMode);
	}

	std::unique_ptr<SolidColorBrush> OpenGLResourceFactory::CreateSolidColorBrush(const Color color)
	{
		return OpenGLSolidColorBrush::Create(color);
	}

	std::unique_ptr<TextureBrush> OpenGLResourceFactory::CreateTextureBrush(const Texture& texture, const TextureSampler& sampler)
	{
		return OpenGLTextureBrush::Create(&texture, &sampler);
	}
}