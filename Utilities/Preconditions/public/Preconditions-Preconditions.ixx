// Project Name : Preconditions
// File Name    : Preconditions-Preconditions.ixx
// Author       : Felix Busch
// Created Date : 2025/10/06

module;

#include <concepts>
#include <stdexcept>

export module Preconditions:Preconditions;

export namespace System
{
	constexpr void Require(const bool value, auto&& lazyMessage) requires std::constructible_from<std::invalid_argument, std::invoke_result_t<decltype(lazyMessage)>>
	{
		if (not value)
		{
			throw std::invalid_argument(lazyMessage());
		}
	}

	constexpr void Require(const bool value)
	{
		Require(value, [] { return "Requirement failed."; });
	}

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

	[[noreturn]] constexpr void Error(auto&& message) requires std::constructible_from<std::runtime_error, decltype(message)>
	{
		throw std::runtime_error(message);
	}
}