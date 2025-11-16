// Project Name : DirectGL
// File Name    : DirectGL-FrameRateLimiter.ixx
// Author       : Felix Busch
// Created Date : 2025/11/16

module;

#include <cstdint>
#include <chrono>

export module DirectGL:FrameRateLimiter;

namespace DGL
{
	class FrameRateLimiter
	{
	public:

		FrameRateLimiter();

		void BeginFrame();
		void EndFrame();

		void SetTargetFrameRate(const uint32_t frameRate);
		uint32_t GetTargetFrameRate() const;

		uint32_t GetCurrentFrameRate() const;

	private:

		void UpdateFPS(const std::chrono::steady_clock::time_point& now);

		uint32_t m_TargetFrameRate;
		std::chrono::duration<double> m_TargetFrameDuration;

		std::chrono::steady_clock::time_point m_FrameStartTime;
		std::chrono::steady_clock::time_point m_LastFPSTimePoint;

		uint32_t m_FramesSinceLastFPSUpdate;
		uint32_t m_CurrentFrameRate;

	};

}