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

	constexpr auto RequireNotNull(auto value, auto&& lazyMessage)
	{
		return static_cast<bool>(value)
			? value
			: throw std::invalid_argument(lazyMessage());
	}

	constexpr auto RequireNotNull(auto value)
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

	constexpr auto CheckNotNull(auto value, auto&& lazyMessage)
	{
		return static_cast<bool>(value)
			? value
			: throw std::runtime_error(lazyMessage());;
	}

	constexpr auto CheckNotNull(auto value)
	{
		return CheckNotNull(value, [] { return "Required value was null."; });
	}
}