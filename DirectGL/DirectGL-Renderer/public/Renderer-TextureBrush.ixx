// Project Name : DirectGL-Renderer
// File Name    : Renderer-TextureBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/10/11

module;

#include <memory>

export module DirectGL.Renderer:TextureBrush;

import :ShaderProgram;
import :Texture;
import :TextureSampler;

export namespace DGL::Renderer
{
	class TextureBrush
	{
	public:

		static std::unique_ptr<TextureBrush> Create();

		void SetTexture(const Texture* texture);
		const Texture* GetTexture() const;

		void SetFilterMode(TextureFilterMode filterMode);
		TextureFilterMode GetFilterMode() const;

		void SetWrapMode(TextureWrapMode wrapMode);
		TextureWrapMode GetWrapMode() const;

		void UploadUniforms(const Math::Matrix4x4& projectionViewMatrix, const Math::Matrix4x4& modelMatrix);

	private:

		explicit TextureBrush(std::unique_ptr<ShaderProgram> shaderProgram);

		std::unique_ptr<ShaderProgram> m_ShaderProgram;
		std::unique_ptr<TextureSampler> m_TextureSampler;

		const Texture* m_Texture;
		
	};
}