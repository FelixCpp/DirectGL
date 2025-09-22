// Project Name : DirectGL
// File Name    : DGL-Shader.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <glad/gl.h>

#include <string_view>

export module DGL:Shader;

namespace DGL
{
	class Shader
	{
	public:

		enum class Type
		{
			Vertex,
			Fragment,
		};

		Shader();
		~Shader();

		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;
		Shader(Shader&&) noexcept;
		Shader& operator=(Shader&&) noexcept;

		[[nodiscard]] bool LoadFromMemory(std::string_view source, Type type);

		GLuint GetShaderId() const;
		Type GetType() const;

	private:

		GLuint m_ShaderId;
		Type m_Type;

	};
}