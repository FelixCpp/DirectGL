// Project Name : DirectGL
// File Name    : DirectGL-ImageSampler.ixx
// Author       : Felix Busch
// Created Date : 2025/11/15

module;

#include <memory>

export module DirectGL:ImageSampler;

import :ImageSamplerFilterMode;
import :ImageSamplerWrapMode;

export namespace DGL
{
	class ImageSampler
	{
	public:
		
		static std::unique_ptr<ImageSampler> Create(
			ImageSamplerFilterMode filterMode = ImageSamplerFilterMode::Linear,
			ImageSamplerWrapMode wrapMode = ImageSamplerWrapMode::ClampToEdge
		);

		~ImageSampler();

		void SetFilterMode(ImageSamplerFilterMode filterMode);
		ImageSamplerFilterMode GetFilterMode() const;

		void SetWrapMode(ImageSamplerWrapMode wrapMode);
		ImageSamplerWrapMode GetWrapMode() const;

		uint32_t GetSamplerId() const;

	private:

		explicit ImageSampler(uint32_t samplerId, ImageSamplerFilterMode filterMode, ImageSamplerWrapMode wrapMode);

		uint32_t m_SamplerId;
		ImageSamplerFilterMode m_FilterMode;
		ImageSamplerWrapMode m_WrapMode;

	};
}