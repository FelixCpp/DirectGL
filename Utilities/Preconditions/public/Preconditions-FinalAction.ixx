// Project Name : Preconditions
// File Name    : Preconditions-FinalAction.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <type_traits>
#include <utility>
#include <concepts>

export module Preconditions:FinalAction;

export namespace System
{
	template <std::invocable Function>
	class FinalAction
	{
	public:

		constexpr explicit FinalAction(Function action) noexcept :
			m_Action(std::move(action)),
			m_ShouldInvoke(true)
		{
		}

		~FinalAction() noexcept
		{
			if (m_ShouldInvoke) m_Action();
		}

		FinalAction(const FinalAction&) = delete;
		FinalAction& operator=(const FinalAction&) = delete;

		FinalAction(FinalAction&& other) noexcept :
			m_Action(std::move(other.m_Action)),
			m_ShouldInvoke(other.m_ShouldInvoke)
		{
			other.m_ShouldInvoke = false;
		}

		FinalAction& operator=(FinalAction&& other) noexcept
		{
			if (this != &other)
			{
				if (m_ShouldInvoke) m_Action();
				m_Action = std::move(other.m_Action);
				m_ShouldInvoke = std::exchange(other.m_ShouldInvoke, false);
			}

			return *this;
		}

	private:

		Function m_Action;
		bool m_ShouldInvoke;

	};

	template <std::invocable Function> constexpr FinalAction<Function> Finally(const Function& function) noexcept { return FinalAction<Function>(function); }
	template <std::invocable Function> constexpr FinalAction<Function> Finally(Function&& function) noexcept { return FinalAction<Function>(std::forward<Function>(function)); }
}