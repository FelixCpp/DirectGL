// Project Name : DirectGL
// File Name    : ControlFlow-Require.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

module;

#include <concepts>
#include <stdexcept>

export module DirectGL.ControlFlow:Require;

export namespace DGL
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
}