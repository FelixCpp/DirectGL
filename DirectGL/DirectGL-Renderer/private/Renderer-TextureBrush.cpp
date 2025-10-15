module;

#include <glad/gl.h>

#include <type_traits>

module DirectGL.Renderer;

import DirectGL.Logging;

inline static constexpr auto VERTEX_SOURCE = R"(
#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

layout (location = 0) out vec2 v_TexCoord;

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_ModelMatrix;

void main() {
	gl_Position = u_ProjectionViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
	v_TexCoord = a_TexCoord;
}
)";

inline static constexpr auto FRAGMENT_SOURCE = R"(
#version 460 core

layout (location = 0) out vec4 o_FragColor;
layout (location = 0) in vec2 v_TexCoord;

layout (binding = 0) uniform sampler2D u_Texture;

void main() {
	o_FragColor = texture(u_Texture, v_TexCoord);
}
)";

namespace DGL::Renderer
{
	std::unique_ptr<TextureBrush> TextureBrush::Create()
	{
		const auto vertexShader = Shader::Create(VERTEX_SOURCE, ShaderType::Vertex);
		if (not vertexShader)
		{
			Logging::Error("Failed to create vertex shader for texture brush");
			return nullptr;
		}

		const auto fragmentShader = Shader::Create(FRAGMENT_SOURCE, ShaderType::Fragment);
		if (not fragmentShader)
		{
			Logging::Error("Failed to create fragment shader for texture brush");
			return nullptr;
		}

		auto shaderProgram = ShaderProgram::Create(*vertexShader, *fragmentShader);
		if (not shaderProgram)
		{
			Logging::Error("Failed to create shader program for texture brush");
			return nullptr;
		}

		return std::unique_ptr<TextureBrush>(new TextureBrush(std::move(shaderProgram)));
	}

	void TextureBrush::SetTexture(const Texture* texture)
	{
		m_Texture = texture;
	}

	const Texture* TextureBrush::GetTexture() const
	{
		return m_Texture;
	}

	void TextureBrush::SetFilterMode(const TextureFilterMode filterMode)
	{
		m_TextureSampler->SetFilterMode(filterMode);
	}

	TextureFilterMode TextureBrush::GetFilterMode() const
	{
		return m_TextureSampler->GetFilterMode();
	}

	void TextureBrush::SetWrapMode(const TextureWrapMode wrapMode)
	{
		m_TextureSampler->SetWrapMode(wrapMode);
	}

	TextureWrapMode TextureBrush::GetWrapMode() const
	{
		return m_TextureSampler->GetWrapMode();
	}

	void TextureBrush::UploadUniforms(const Math::Matrix4x4& projectionViewMatrix, const Math::Matrix4x4& modelMatrix)
	{
		if (m_Texture == nullptr)
		{
			Logging::Warning("TextureBrush: No texture set, cannot upload uniforms");
			return;
		}

		m_ShaderProgram->UploadTexture("u_Texture", 0);
		m_ShaderProgram->UploadMatrix4x4("u_ProjectionViewMatrix", std::span<const float, 16>(projectionViewMatrix.GetData(), 16));
		m_ShaderProgram->UploadMatrix4x4("u_ModelMatrix", std::span<const float, 16>(modelMatrix.GetData(), 16));

		glBindSampler(0, m_TextureSampler->GetRendererId());
		glBindTextureUnit(0, m_Texture->GetRendererId());
		ShaderProgram::Activate(m_ShaderProgram.get());
	}

	TextureBrush::TextureBrush(std::unique_ptr<ShaderProgram> shaderProgram):
		m_ShaderProgram(std::move(shaderProgram)),
		m_TextureSampler(TextureSampler::Create()),
		m_Texture(nullptr)
	{
	}
}