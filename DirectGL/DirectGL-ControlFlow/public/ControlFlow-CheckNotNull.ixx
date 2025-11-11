// Project Name : DirectGL
// File Name    : ControlFlow-CheckNotNull.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

module;

#include <concepts>
#include <stdexcept>

export module DirectGL.ControlFlow:CheckNotNull;

export namespace DGL
{
	template <std::convertible_to<bool> T>
	constexpr T CheckNotNull(T value, auto&& lazyMessage) requires std::constructible_from<std::runtime_error, std::invoke_result_t<decltype(lazyMessage)>>
	{
		return static_cast<bool>(value)
			? value
			: throw std::runtime_error(lazyMessage());
	}

	template <std::convertible_to<bool> T>
	constexpr T CheckNotNull(T value)
	{
		return CheckNotNull(value, [] { return "Required value was null."; });
	}
}