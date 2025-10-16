// Project Name : DirectGL-Brushes
// File Name    : Brushes-ShaderProgram.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

module;

#include <glad/gl.h>

#include <memory>
#include <string_view>
#include <unordered_map>
#include <span>

export module DirectGL.Brushes:ShaderProgram;

import :Shader;

namespace DGL::Brushes
{
	class ShaderProgram
	{
	public:

		static std::unique_ptr<ShaderProgram> Create(const Shader& vertexShader, const Shader& fragmentShader);

		~ShaderProgram();

		void UploadTexture(std::string_view name, GLuint textureUnit);

		void UploadFloat1(std::string_view name, float x);
		void UploadFloat2(std::string_view name, float x, float y);
		void UploadFloat3(std::string_view name, float x, float y, float z);
		void UploadFloat4(std::string_view name, float x, float y, float z, float w);

		void UploadMatrix4x4(std::string_view name, const std::span<const float, 16>& matrix);

		GLuint GetRendererId() const;

		static void Activate(const ShaderProgram* program);

	private:

		explicit ShaderProgram(GLuint rendererId);

		GLint GetUniformLocation(std::string_view name);

		std::unordered_map<std::string_view, GLint> m_UniformLocationCache;
		GLuint m_RendererId;

	};
}