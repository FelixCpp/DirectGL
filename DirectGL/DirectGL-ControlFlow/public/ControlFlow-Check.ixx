// Project Name : DirectGL
// File Name    : ControlFlow-Check.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

module;

#include <concepts>
#include <stdexcept>

export module DirectGL.ControlFlow:Check;

export namespace DGL
{
	constexpr auto Check(const bool value, auto&& lazyMessage) requires std::constructible_from<std::runtime_error, std::invoke_result_t<decltype(lazyMessage)>>
	{
		if (not value)
		{
			throw std::runtime_error(lazyMessage());
		}
	}

	constexpr auto Check(const bool value)
	{
		Check(value, [] { return "Check failed."; });
	}
}