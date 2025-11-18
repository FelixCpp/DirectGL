// Project Name : DirectGL
// File Name    : DirectGL-Shader.ixx
// Author       : Felix Busch
// Created Date : 2025/11/17

module;

#include <cstdint>
#include <memory>
#include <string_view>
#include <filesystem>
#include <unordered_map>

export module DirectGL:Shader;

export namespace DGL
{
	class Shader
	{
	public:

		static std::unique_ptr<Shader> CreateFromSource(std::string_view vertexSource, std::string_view fragmentSource);
		static std::unique_ptr<Shader> CreateFromFiles(const std::filesystem::path& vertexFilePath, const std::filesystem::path& fragmentFilePath);

		~Shader();

		void UploadInt1(std::string_view name, int32_t value);
		void UploadInt2(std::string_view name, int32_t value1, int32_t value2);
		void UploadInt3(std::string_view name, int32_t value1, int32_t value2, int32_t value3);
		void UploadInt4(std::string_view name, int32_t value1, int32_t value2, int32_t value3, int32_t value4);

		void UploadFloat1(std::string_view name, float value);
		void UploadFloat2(std::string_view name, float value1, float value2);
		void UploadFloat3(std::string_view name, float value1, float value2, float value3);
		void UploadFloat4(std::string_view name, float value1, float value2, float value3, float value4);

		void UploadFloatMatrix4x4(std::string_view name, const float* matrixData);

		uint32_t GetShaderId() const;

	private:

		int32_t GetUniformLocation(std::string_view name);

		explicit Shader(uint32_t shaderId);

		std::unordered_map<std::string_view, int32_t> m_UniformLocationCache;
		uint32_t m_ShaderId;

	};
}