// Project Name : DirectGL
// File Name    : DGL-OpenGLShaderProgram.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

module;

#include <glad/gl.h>

#include <memory>
#include <string_view>

export module DGL:OpenGLShaderProgram;

import :ShaderProgram;
import :Shader;

namespace DGL
{
	class OpenGLShaderProgram : public ShaderProgram
	{
	public:

		static std::unique_ptr<OpenGLShaderProgram> Create(const Shader& vertexShader, const Shader& fragmentShader);

		void UploadInt1(std::string_view name, std::int32_t x) override;
		void UploadInt2(std::string_view name, std::int32_t x, std::int32_t y) override;
		void UploadInt3(std::string_view name, std::int32_t x, std::int32_t y, std::int32_t z) override;
		void UploadInt4(std::string_view name, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t w) override;

		void UploadFloat1(std::string_view name, float x) override;
		void UploadFloat2(std::string_view name, float x, float y) override;
		void UploadFloat3(std::string_view name, float x, float y, float z) override;
		void UploadFloat4(std::string_view name, float x, float y, float z, float w) override;

		void UploadFloat1Array(std::string_view name, std::span<const float, 1> data) override;
		void UploadFloat2Array(std::string_view name, std::span<const float, 2> data) override;
		void UploadFloat3Array(std::string_view name, std::span<const float, 3> data) override;
		void UploadFloat4Array(std::string_view name, std::span<const float, 4> data) override;

		void UploadMatrix4x4(std::string_view name, std::span<const float, 16> matrix) override;

		void Bind();

	private:

		explicit OpenGLShaderProgram(GLuint shaderProgramId);

		[[nodiscard]] GLint GetUniformLocation(std::string_view name);

		GLuint m_ShaderProgramId;

	};
}