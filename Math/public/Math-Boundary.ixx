module;

#include <cstdint>

export module Math:Boundary;

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

		constexpr bool operator == (const Boundary& other) const;
		constexpr bool operator != (const Boundary& other) const;

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
	constexpr bool Boundary<T>::operator==(const Boundary<T>& other) const
	{
		return Left == other.Left and Top == other.Top and Width == other.Width and Height == other.Height;
	}

	template <typename T>
	constexpr bool Boundary<T>::operator!=(const Boundary<T>& other) const
	{
		return Left != other.Left or Top != other.Top or Width != other.Width or Height != other.Height;
	}
}