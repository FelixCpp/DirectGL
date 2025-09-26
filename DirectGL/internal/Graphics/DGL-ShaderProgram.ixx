// Project Name : DirectGL
// File Name    : DGL-ShaderProgram.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <glad/gl.h>

#include <string_view>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <memory>

export module DGL:ShaderProgram;

import :Shader;

namespace DGL
{
	class ShaderProgram
	{
	public:

		static std::unique_ptr<ShaderProgram> Create(const Shader& vertexShader, const Shader& fragmentShader);

		~ShaderProgram();

		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;
		ShaderProgram(ShaderProgram&&) noexcept;
		ShaderProgram& operator=(ShaderProgram&&) noexcept;

		[[nodiscard]] bool Load(const Shader& vertexShader, const Shader& fragmentShader);

		void Bind();
		void UploadInt1(std::string_view name, std::int32_t x);

		void UploadFloat1Array(std::string_view name, const float* data, std::size_t count);
		void UploadFloat4Array(std::string_view name, const float* data, std::size_t count);

		void UploadFloat2(std::string_view name, float x, float y);
		void UploadFloat3(std::string_view name, float x, float y, float z);
		void UploadFloat4(std::string_view name, float x, float y, float z, float w);

	private:

		explicit ShaderProgram(GLuint shaderProgramId);
		[[nodiscard]] GLint GetUniformLocation(std::string_view name);

		GLuint m_ShaderProgramId;
		std::unordered_map<std::string_view, GLint> m_UniformLocationCache;

	};
}