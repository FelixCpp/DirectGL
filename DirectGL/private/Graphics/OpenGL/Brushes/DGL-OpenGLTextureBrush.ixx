// Project Name : DirectGL
// File Name    : DGL-OpenGLTextureBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/09/28

module;

#include <memory>

export module DGL:OpenGLTextureBrush;

import :TextureBrush;

import :OpenGLTexture;
import :OpenGLTextureSampler;
import :OpenGLShaderProgram;

namespace DGL
{
	class OpenGLTextureBrush : public TextureBrush
	{
	public:

		[[nodiscard]] static std::unique_ptr<OpenGLTextureBrush> Create(const Texture* texture, const TextureSampler* sampler);

		void SetTexture(const Texture* texture) override;
		[[nodiscard]] const Texture* GetTexture() const override;

		void SetTextureSampler(const TextureSampler* textureSampler) override;
		[[nodiscard]] const TextureSampler* GetTextureSampler() const override;

		void SetSourceRect(const Math::FloatBoundary& boundary) override;
		[[nodiscard]] const Math::FloatBoundary& GetSourceRect() const override;

		void Apply(const Camera& camera) override;

	private:

		explicit OpenGLTextureBrush(
			std::unique_ptr<OpenGLShaderProgram> shaderProgram,
			const OpenGLTexture* texture,
			const OpenGLTextureSampler* sampler
		);

		const OpenGLTexture* m_Texture;
		const OpenGLTextureSampler* m_Sampler;
		Math::FloatBoundary m_SourceRect;
		std::unique_ptr<OpenGLShaderProgram> m_ShaderProgram;

	};
}