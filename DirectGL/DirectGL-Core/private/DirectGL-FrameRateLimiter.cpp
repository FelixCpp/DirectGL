module;

#include <Windows.h>
#include <chrono>
#include <format>

#pragma comment(lib, "Winmm.lib")

module DirectGL;

import :FrameRateLimiter;

void SleepForMilliseconds(const uint32_t milliseconds)
{
	static const UINT periodMin = []
	{
		TIMECAPS timeCaps;
		timeGetDevCaps(&timeCaps, sizeof(TIMECAPS));
		return timeCaps.wPeriodMin;
	}();

	timeBeginPeriod(periodMin);
	Sleep(milliseconds);
	timeEndPeriod(periodMin);
}

namespace DGL
{
	FrameRateLimiter::FrameRateLimiter() :
		m_TargetFrameRate(60),
		m_TargetFrameDuration(std::chrono::duration<double>(1.0 / static_cast<double>(m_TargetFrameRate))),
		m_FramesSinceLastFPSUpdate(0),
		m_CurrentFrameRate(0)
	{
	}

	void FrameRateLimiter::BeginFrame()
	{
		m_FrameStartTime = std::chrono::steady_clock::now();
	}

	void FrameRateLimiter::EndFrame()
	{
		const auto now = std::chrono::steady_clock::now();
		const auto frameDuration = now - m_FrameStartTime;

		UpdateFPS(now);

		if (m_TargetFrameRate > 0)
		{
			const auto frameDurationInSeconds = std::chrono::duration<double>(frameDuration);
			if (frameDurationInSeconds < m_TargetFrameDuration)
			{
				const auto sleepDuration = m_TargetFrameDuration - frameDurationInSeconds;
				const uint32_t sleepMilliseconds = static_cast<uint32_t>(sleepDuration.count() * 1000.0);

				if (sleepMilliseconds > 0)
				{
					SleepForMilliseconds(sleepMilliseconds);
				}
			}
		}

	}

	void FrameRateLimiter::SetTargetFrameRate(const uint32_t frameRate)
	{
		m_TargetFrameRate = frameRate;
		m_TargetFrameDuration = std::chrono::duration<double>(1.0 / static_cast<double>(m_TargetFrameRate));
	}

	uint32_t FrameRateLimiter::GetTargetFrameRate() const
	{
		return m_TargetFrameRate;
	}

	uint32_t FrameRateLimiter::GetCurrentFrameRate() const
	{
		return m_CurrentFrameRate;
	}

	void FrameRateLimiter::UpdateFPS(const std::chrono::steady_clock::time_point& now)
	{
		++m_FramesSinceLastFPSUpdate;
		const auto timeSinceLastFPSUpdate = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_LastFPSTimePoint).count();

		if (timeSinceLastFPSUpdate >= 0.25)
		{
			const double fps = static_cast<double>(m_FramesSinceLastFPSUpdate) / timeSinceLastFPSUpdate;
			m_CurrentFrameRate = static_cast<uint32_t>(fps + 0.5);
			m_FramesSinceLastFPSUpdate = 0;
			m_LastFPSTimePoint = now;

			Debug(std::format("Current FPS: {}", m_CurrentFrameRate));
		}
	}

}