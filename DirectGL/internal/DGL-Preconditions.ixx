module;

#include <stdexcept>
#include <format>

export module DGL:Preconditions;

namespace DGL
{
	constexpr void Require(const bool value, auto&& lazyMessage)
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
	constexpr T RequireNotNull(T value, auto&& lazyMessage)
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

	constexpr auto Check(const bool value, auto&& lazyMessage)
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
	constexpr T CheckNotNull(T value, auto&& lazyMessage)
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