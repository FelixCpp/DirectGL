module;

#include <cstdint>

export module Math:Boundary;

import :Value2;

namespace Math
{
	export template <typename T>
	struct Boundary
	{
		static constexpr Boundary FromLTRB(T left, T top, T right, T bottom);
		static constexpr Boundary FromLTWH(T left, T top, T width, T height);

		constexpr Boundary WithPosition(T left, T top) const;
		constexpr Boundary WithSize(T width, T height) const;

		constexpr T Right() const;
		constexpr T Bottom() const;

		constexpr bool operator == (const Boundary& other) const = default;
		constexpr bool operator != (const Boundary& other) const = default;

		[[nodiscard]] constexpr Value2<T> TopLeft() const;
		[[nodiscard]] constexpr Value2<T> TopRight() const;
		[[nodiscard]] constexpr Value2<T> BottomLeft() const;
		[[nodiscard]] constexpr Value2<T> BottomRight() const;
		[[nodiscard]] constexpr Value2<T> Center() const;

		T Left;
		T Top;
		T Width;
		T Height;
	};

	export typedef Boundary<float> FloatBoundary;
	export typedef Boundary<int32_t> IntBoundary;
	export typedef Boundary<uint32_t> UintBoundary;
}

namespace Math
{
	template <typename T>
	constexpr Boundary<T> Boundary<T>::FromLTRB(const T left, const T top, const T right, const T bottom)
	{
		return { .Left = left, .Top = top, .Width = right - left, .Height = bottom - top };
	}

	template <typename T>
	constexpr Boundary<T> Boundary<T>::FromLTWH(const T left, const T top, const T width, const T height)
	{
		return { .Left = left, .Top = top, .Width = width, .Height = height };
	}

	template <typename T>
	constexpr Boundary<T> Boundary<T>::WithPosition(const T left, const T top) const
	{
		return { .Left = left, .Top = top, .Width = Width, .Height = Height };
	}

	template <typename T>
	constexpr Boundary<T> Boundary<T>::WithSize(const T width, const T height) const
	{
		return { .Left = Left, .Top = Top, .Width = width, .Height = height };
	}

	template <typename T>
	constexpr T Boundary<T>::Right() const
	{
		return Left + Width;
	}

	template <typename T>
	constexpr T Boundary<T>::Bottom() const
	{
		return Top + Height;
	}

	template <typename T>
	constexpr Value2<T> Boundary<T>::TopLeft() const
	{
		return { Left, Top };
	}

	template <typename T>
	constexpr Value2<T> Boundary<T>::TopRight() const
	{
		return { Right(), Top };
	}

	template <typename T>
	constexpr Value2<T> Boundary<T>::BottomLeft() const
	{
		return { Left, Bottom() };
	}

	template <typename T>
	constexpr Value2<T> Boundary<T>::BottomRight() const
	{
		return { Right(), Bottom() };
	}

	template <typename T>
	constexpr Value2<T> Boundary<T>::Center() const
	{
		return { Left + Width / 2, Top + Height / 2 };
	}
}