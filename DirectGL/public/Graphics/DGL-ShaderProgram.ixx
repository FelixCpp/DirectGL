// Project Name : DirectGL
// File Name    : DGL-ShaderProgram.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

module;

#include <string_view>
#include <cstdint>
#include <span>

export module DGL:ShaderProgram;

export namespace DGL
{
	struct ShaderProgram
	{
		virtual ~ShaderProgram() = default;

		virtual void UploadInt1(std::string_view name, std::int32_t x) = 0;
		virtual void UploadInt2(std::string_view name, std::int32_t x, std::int32_t y) = 0;
		virtual void UploadInt3(std::string_view name, std::int32_t x, std::int32_t y, std::int32_t z) = 0;
		virtual void UploadInt4(std::string_view name, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t w) = 0;

		virtual void UploadFloat1(std::string_view name, float x) = 0;
		virtual void UploadFloat2(std::string_view name, float x, float y) = 0;
		virtual void UploadFloat3(std::string_view name, float x, float y, float z) = 0;
		virtual void UploadFloat4(std::string_view name, float x, float y, float z, float w) = 0;

		virtual void UploadFloat1Array(std::string_view name, std::span<const float, 1> data) = 0;
		virtual void UploadFloat2Array(std::string_view name, std::span<const float, 2> data) = 0;
		virtual void UploadFloat3Array(std::string_view name, std::span<const float, 3> data) = 0;
		virtual void UploadFloat4Array(std::string_view name, std::span<const float, 4> data) = 0;

		virtual void UploadMatrix4x4(std::string_view name, std::span<const float, 16> matrix) = 0;
	};
}