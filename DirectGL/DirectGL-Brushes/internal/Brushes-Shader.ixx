// Project Name : DirectGL-Brushes
// File Name    : Brushes-Shader.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

module;

#include <Glad/gl.h>

#include <memory>
#include <string_view>

export module DirectGL.Brushes:Shader;

namespace DGL::Brushes
{
	enum class ShaderType
	{
		Vertex,
		Fragment,
		Geometry,
	};

	class Shader
	{
	public:

		static std::unique_ptr<Shader> Create(std::string_view source, ShaderType type);

		~Shader();

		ShaderType GetType() const;
		GLuint GetRendererId() const;

	private:

		explicit Shader(GLuint rendererId, ShaderType type);

		GLuint m_RendererId;
		ShaderType m_Type;

	};
}
