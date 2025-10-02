// Project Name : DirectGL
// File Name    : DGL-OpenGLResourceFactory.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

module;

#include <memory>
#include <cstdint>
#include <string_view>

export module DGL:OpenGLResourceFactory;

import :ResourceFactory;

namespace DGL
{
	struct OpenGLResourceFactory : ResourceFactory
	{
		std::unique_ptr<WindowRenderTarget> CreateWindowRenderTarget(System::Window& window, Renderer& renderer) override;
		std::unique_ptr<OffscreenRenderTarget> CreateFramebuffer(uint32_t width, uint32_t height, Renderer& renderer) override;

		std::unique_ptr<Renderer> CreateRenderer(uint32_t maxVertices) override;
		std::unique_ptr<Shader> CreateShader(std::string_view source, ShaderType type) override;
		std::unique_ptr<ShaderProgram> CreateShaderProgram(const Shader& vertexShader, const Shader& fragmentShader) override;
		std::unique_ptr<Texture> CreateTexture(const std::filesystem::path& filepath) override;
		std::unique_ptr<TextureSampler> CreateTextureSampler(TextureWrapMode wrapMode, TextureFilterMode filterMode) override;

		std::unique_ptr<SolidColorBrush> CreateSolidColorBrush(Color color) override;
		std::unique_ptr<TextureBrush> CreateTextureBrush(const Texture& texture, const TextureSampler& sampler) override;
	};
}