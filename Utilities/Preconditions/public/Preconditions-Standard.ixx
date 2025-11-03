// Project Name : Preconditions
// File Name    : Preconditions-Standard.ixx
// Author       : Felix Busch
// Created Date : 2025/10/06

module;

#include <stdexcept>
#include <format>

export module Preconditions:Standard;

export namespace System
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