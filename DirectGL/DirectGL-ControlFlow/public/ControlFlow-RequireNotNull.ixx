// Project Name : DirectGL
// File Name    : ControlFlow-RequireNotNull.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

module;

#include <concepts>
#include <stdexcept>

export module DirectGL.ControlFlow:RequireNotNull;

export namespace DGL
{
	template <std::convertible_to<bool> T>
	constexpr T RequireNotNull(T value, auto&& lazyMessage) requires std::constructible_from<std::invalid_argument, std::invoke_result_t<decltype(lazyMessage)>>
	{
		return static_cast<bool>(value)
			? value
			: throw std::invalid_argument(lazyMessage());
	}

	template <std::convertible_to<bool> T>
	constexpr T RequireNotNull(T value)
	{
		return RequireNotNull(value, [] { return "Required value was null."; });
	}
}