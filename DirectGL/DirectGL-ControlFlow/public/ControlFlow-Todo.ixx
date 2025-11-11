// Project Name : DirectGL
// File Name    : ControlFlow-Todo.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

module;

#include <stdexcept>
#include <format>

export module DirectGL.ControlFlow:Todo;

export namespace DGL
{
	[[noreturn]] inline void TODO()
	{
		throw std::domain_error("An operation is not yet implemented");
	}

	[[noreturn]] void TODO(auto&& reason) requires std::constructible_from<std::string, decltype(reason)>
	{
		throw std::domain_error(std::format("An operation is not yet implemented: {}", reason));
	}
}