// Project Name : DirectGL
// File Name    : DGL-Shader.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

export module DGL:Shader;

export namespace DGL
{
	enum class ShaderType
	{
		Vertex,
		Fragment,
		Geometry,
		Compute,
	};

	struct Shader
	{
		virtual ~Shader() = default;
		[[nodiscard]] virtual ShaderType GetShaderType() const = 0;
	};
}