// Project Name : DirectGL
// File Name    : DGL-ResourceFactory.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

module;

#include <memory>
#include <filesystem>

export module DGL:ResourceFactory;

import :Shader;
import :ShaderProgram;
import :Renderer;
import :Texture;
import :TextureSampler;

import :WindowRenderTarget;
import :OffscreenRenderTarget;

import :SolidColorBrush;
import :TextureBrush;

namespace DGL
{
	struct ResourceFactory
	{
		virtual ~ResourceFactory() = default;

		virtual std::unique_ptr<WindowRenderTarget> CreateWindowRenderTarget(System::Window& window, Renderer& renderer) = 0;
		virtual std::unique_ptr<OffscreenRenderTarget> CreateFramebuffer(uint32_t width, uint32_t height, Renderer& renderer) = 0;

		virtual std::unique_ptr<Renderer> CreateRenderer(uint32_t maxVertices) = 0;
		virtual std::unique_ptr<Shader> CreateShader(std::string_view source, ShaderType type) = 0;
		virtual std::unique_ptr<ShaderProgram> CreateShaderProgram(const Shader& vertexShader, const Shader& fragmentShader) = 0;
		virtual std::unique_ptr<Texture> CreateTexture(const std::filesystem::path& filepath) = 0;
		virtual std::unique_ptr<TextureSampler> CreateTextureSampler(TextureWrapMode wrapMode, TextureFilterMode filterMode) = 0;

		virtual std::unique_ptr<SolidColorBrush> CreateSolidColorBrush(Color color) = 0;
	};
}