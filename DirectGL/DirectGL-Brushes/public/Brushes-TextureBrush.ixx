// Project Name : DirectGL-Brushes
// File Name    : Brushes-TextureBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/10/11

module;

#include <memory>

export module DirectGL.Brushes:TextureBrush;

import :ShaderProgram;

import DirectGL.Math;
import DirectGL.Texture;

export namespace DGL::Brushes
{
	class TextureBrush
	{
	public:

		static std::unique_ptr<TextureBrush> Create();

		void SetTexture(const Texture::Texture* texture);
		const Texture::Texture* GetTexture() const;

		void SetFilterMode(Texture::TextureFilterMode filterMode);
		Texture::TextureFilterMode GetFilterMode() const;

		void SetWrapMode(Texture::TextureWrapMode wrapMode);
		Texture::TextureWrapMode GetWrapMode() const;

		void UploadUniforms(const Math::Matrix4x4& projectionViewMatrix, const Math::Matrix4x4& modelMatrix);

	private:

		explicit TextureBrush(std::unique_ptr<ShaderProgram> shaderProgram);

		std::unique_ptr<ShaderProgram> m_ShaderProgram;
		std::unique_ptr<Texture::TextureSampler> m_TextureSampler;

		const Texture::Texture* m_Texture;
		
	};
}