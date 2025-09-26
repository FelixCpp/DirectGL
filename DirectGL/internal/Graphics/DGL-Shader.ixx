// Project Name : DirectGL
// File Name    : DGL-Shader.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <glad/gl.h>

#include <string_view>
#include <memory>

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

		static std::unique_ptr<Shader> CreateFromSource(std::string_view source, Type type);
		~Shader();

		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;
		Shader(Shader&&) noexcept;
		Shader& operator=(Shader&&) noexcept;

		GLuint GetShaderId() const;
		Type GetType() const;

	private:

		explicit Shader(GLuint shaderId, Type type);

		GLuint m_ShaderId;
		Type m_Type;

	};
}