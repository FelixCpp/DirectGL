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
		std::unique_ptr<Renderer> CreateRenderer(uint32_t maxVertices) override;
		std::unique_ptr<Shader> CreateShader(std::string_view source, ShaderType type) override;
		std::unique_ptr<ShaderProgram> CreateShaderProgram(const Shader& vertexShader, const Shader& fragmentShader) override;
		std::unique_ptr<Texture> CreateTexture(const std::filesystem::path& filepath) override;
		std::unique_ptr<TextureSampler> CreateTextureSampler() override;
		std::unique_ptr<UniformBuffer> CreateUniformBuffer(const void* data, uint32_t size) override;

		std::unique_ptr<SolidColorBrush> CreateSolidColorBrush(const Color color) override;
		std::unique_ptr<TextureBrush> CreateTextureBrush(const Texture& texture, const TextureSampler& sampler) override;
	};
}