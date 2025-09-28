module;

#include <memory>

module DGL;

inline static constexpr auto VertexSource = R"(
#version 460 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout (location = 0) out vec2 v_TexCoord;

uniform mat4 u_ViewProjection;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * vec4(a_Position, 0.0, 1.0);
}
)";

inline static constexpr auto FragmentSource = R"(
#version 460 core

layout (location = 0) out vec4 o_FragColor;

layout (location = 0) in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D u_Texture;

void main()
{
	o_FragColor = texture(u_Texture, v_TexCoord);
}
)";

namespace DGL
{
	std::unique_ptr<OpenGLTextureBrush> OpenGLTextureBrush::Create(const Texture* texture, const TextureSampler* sampler)
	{
		const auto vertexShader = OpenGLShader::Create(VertexSource, ShaderType::Vertex);
		if (vertexShader == nullptr)
		{
			Error("Failed to create vertex shader for texture brush");
			return nullptr;
		}

		const auto fragmentShader = OpenGLShader::Create(FragmentSource, ShaderType::Fragment);
		if (fragmentShader == nullptr)
		{
			Error("Failed to create fragment shader for texture brush");
			return nullptr;
		}

		auto shaderProgram = OpenGLShaderProgram::Create(*vertexShader, *fragmentShader);
		if (shaderProgram == nullptr)
		{
			Error("Failed to create shader program for texture brush");
			return nullptr;
		}

		return std::unique_ptr<OpenGLTextureBrush>(new OpenGLTextureBrush(
			std::move(shaderProgram),
			dynamic_cast<const OpenGLTexture*>(texture),
			dynamic_cast<const OpenGLTextureSampler*>(sampler)
		));
	}

	void OpenGLTextureBrush::SetTexture(const Texture* texture)
	{
		m_Texture = dynamic_cast<const OpenGLTexture*>(texture);
	}

	const Texture* OpenGLTextureBrush::GetTexture() const
	{
		return m_Texture;
	}

	void OpenGLTextureBrush::SetTextureSampler(const TextureSampler* textureSampler)
	{
		m_Sampler = dynamic_cast<const OpenGLTextureSampler*>(textureSampler);
	}

	const TextureSampler* OpenGLTextureBrush::GetTextureSampler() const
	{
		return m_Sampler;
	}

	void OpenGLTextureBrush::Apply(const Camera& camera)
	{
		// Activate texture & sampler for the use inside the shader.
		// Note that we're using slot 0 for binding reference.
		m_Texture->Bind(0);
		m_Sampler->Bind(0);

		// Activate shader and upload uniforms.
		m_ShaderProgram->Bind();
		m_ShaderProgram->UploadInt1("u_Texture", 0);
		m_ShaderProgram->UploadMatrix4x4("u_ViewProjection", std::span<const float, 16>(camera.GetViewProjectionMatrix().GetData(), 16));
	}

	OpenGLTextureBrush::OpenGLTextureBrush(std::unique_ptr<OpenGLShaderProgram> shaderProgram, const OpenGLTexture* texture, const OpenGLTextureSampler* sampler):
		m_Texture(texture),
		m_Sampler(sampler),
		m_ShaderProgram(std::move(shaderProgram))
	{
	}
}