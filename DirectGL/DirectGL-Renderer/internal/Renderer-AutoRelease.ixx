// Project Name : DirectGL-Renderer
// File Name    : Renderer-AutoRelease.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <optional>
#include <functional>
#include <utility>

export module DirectGL.Renderer:AutoRelease;

import Preconditions;

namespace DGL::Renderer
{
	template <typename T>
	class AutoRelease
	{
	public:

		AutoRelease(T value, std::function<void(T)> release);
		~AutoRelease();

		AutoRelease(const AutoRelease&) = delete;
		AutoRelease& operator=(const AutoRelease&) = delete;

		AutoRelease(AutoRelease&& other) noexcept;
		AutoRelease& operator = (AutoRelease&& other) noexcept;

		T Get() const { return m_Value; }

	private:

		T m_Value;
		std::function<void(T)> m_Release;
		bool m_ShouldInvoke;

	};
}

namespace DGL::Renderer
{
	template <typename T>
	AutoRelease<T>::AutoRelease(T value, std::function<void(T)> release):
		m_Value(value),
		m_Release(std::move(release)),
		m_ShouldInvoke(true)
	{
	}

	template <typename T>
	AutoRelease<T>::~AutoRelease()
	{
		if (m_ShouldInvoke) m_Release(m_Value);
	}

	template <typename T>
	AutoRelease<T>::AutoRelease(AutoRelease&& other) noexcept:
		m_Value(std::move(other.m_Value)),
		m_Release(std::move(other.m_Release)),
		m_ShouldInvoke(std::exchange(other.m_ShouldInvoke, false))
	{
	}

	template <typename T>
	AutoRelease<T>& AutoRelease<T>::operator=(AutoRelease&& other) noexcept
	{
		if (this != &other)
		{
			if (m_ShouldInvoke) m_Release(m_Value);
			m_Value = std::move(other.m_Value);
			m_Release = std::move(other.m_Release);
			m_ShouldInvoke = std::exchange(other.m_ShouldInvoke, false);
		}

		return *this;
	}
}