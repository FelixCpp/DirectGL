// Project Name : DirectGL
// File Name    : DirectGL-ClipRect.ixx
// Author       : Felix Busch
// Created Date : 2025/11/18

module;

#include <optional>

export module DirectGL:ClipRect;

import DirectGL.Math;

export namespace DGL
{
	struct ClipRect
	{
	public:

		static constexpr ClipRect Unclipped();
		static constexpr ClipRect Clipped(const Math::IntBoundary& boundary);
		static constexpr ClipRect Optional(const Math::IntBoundary& boundary, bool enabled);
		static constexpr ClipRect Optional(const std::optional<Math::IntBoundary>& boundary);

		constexpr bool operator == (const ClipRect& other) const = default;
		constexpr bool operator != (const ClipRect& other) const = default;

		[[nodiscard]] constexpr bool IsClipped() const;
		[[nodiscard]] constexpr Math::IntBoundary& UnwrapBoundary() &;
		[[nodiscard]] constexpr const Math::IntBoundary& UnwrapBoundary() const&;

	private:

		constexpr explicit ClipRect(const std::optional<Math::IntBoundary>& boundary);

		std::optional<Math::IntBoundary> m_Boundary;

	};
}

namespace DGL
{
	constexpr ClipRect ClipRect::Unclipped()
	{
		return ClipRect{ std::nullopt };
	}

	constexpr ClipRect ClipRect::Clipped(const Math::IntBoundary& boundary)
	{
		return ClipRect{ boundary };
	}

	constexpr ClipRect ClipRect::Optional(const Math::IntBoundary& boundary, const bool enabled)
	{
		if (enabled)
		{
			return Clipped(boundary);
		}

		return Unclipped();
	}

	constexpr ClipRect ClipRect::Optional(const std::optional<Math::IntBoundary>& boundary)
	{
		if (boundary.has_value())
		{
			return Clipped(boundary.value());
		}

		return Unclipped();
	}

	constexpr bool ClipRect::IsClipped() const
	{
		return m_Boundary.has_value();
	}

	constexpr Math::IntBoundary& ClipRect::UnwrapBoundary() &
	{
		return m_Boundary.value();
	}

	constexpr const Math::IntBoundary& ClipRect::UnwrapBoundary() const&
	{
		return m_Boundary.value();
	}

	constexpr ClipRect::ClipRect(const std::optional<Math::IntBoundary>& boundary):
		m_Boundary(boundary)
	{
	}
}