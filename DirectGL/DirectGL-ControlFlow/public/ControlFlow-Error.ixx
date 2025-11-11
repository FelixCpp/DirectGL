// Project Name : DirectGL
// File Name    : ControlFlow-Error.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

module;

#include <concepts>
#include <stdexcept>

export module DirectGL.ControlFlow:Error;

export namespace DGL
{
	[[noreturn]] constexpr void ThrowError(auto&& message) requires std::constructible_from<std::runtime_error, decltype(message)>
	{
		throw std::runtime_error(message);
	}
}