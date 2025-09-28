// Project Name : Math
// File Name    : Math-Matrix4x4.ixx
// Author       : Felix Busch
// Created Date : 2025/09/26

module;

#include <array>

export module Math:Matrix4x4;

import :Boundary;

export namespace Math
{
	class Matrix4x4
	{
	public:

		constexpr Matrix4x4();

		constexpr Matrix4x4(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33
		);

		const float* GetData() const;

		Matrix4x4 operator * (const Matrix4x4& other) const;

		static constexpr Matrix4x4 Translation(float x, float y, float z);
		static constexpr Matrix4x4 Scaling(float x, float y, float z);
		static constexpr Matrix4x4 Orthographic(const FloatBoundary& boundary, float near, float far);

		static const Matrix4x4 Identity;

	private:

		std::array<float, 16> m_Data;

	};
}

namespace Math
{
	constexpr Matrix4x4::Matrix4x4():
		m_Data({
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		})
	{}

	constexpr Matrix4x4::Matrix4x4(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23,
		const float m30, const float m31, const float m32, const float m33
	) : m_Data({
		m00, m10, m20, m30,
		m01, m11, m21, m31,
		m02, m12, m22, m32,
		m03, m13, m23, m33,
	}) {}

	const float* Matrix4x4::GetData() const
	{
		return m_Data.data();
	}

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
	{
		const float* a = GetData();
		const float* b = other.GetData();

		return Matrix4x4(
			a[0] * b[0]  + a[4] * b[1]  + a[8] * b[2]  + a[12] * b[3],
			a[0] * b[4]  + a[4] * b[5]  + a[8] * b[6]  + a[12] * b[7],
			a[0] * b[8]  + a[4] * b[9]  + a[8] * b[10] + a[12] * b[11],
			a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15],

			a[1] * b[0]  + a[5] * b[1]  + a[9] * b[2]  + a[13] * b[3],
			a[1] * b[4]  + a[5] * b[5]  + a[9] * b[6]  + a[13] * b[7],
			a[1] * b[8]  + a[5] * b[9]  + a[9] * b[10] + a[13] * b[11],
			a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15],
			
			a[2] * b[0]  + a[6] * b[1]  + a[10] * b[2]  + a[14] * b[3],
			a[2] * b[4]  + a[6] * b[5]  + a[10] * b[6]  + a[14] * b[7],
			a[2] * b[8]  + a[6] * b[9]  + a[10] * b[10] + a[14] * b[11],
			a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14]* b [15],
			
			a[3] * b[0]  + a[7] * b[1]  + a[11] * b[2]  + a[15] * b[3],
			a[3] * b[4]  + a[7] * b[5]  + a[11] * b[6]  + a[15] * b[7],
			a[3] * b[8]  + a[7] * b[9]  + a[11] * b[10] + a[15] * b[11],
			a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15]
		);
	}

	constexpr Matrix4x4 Matrix4x4::Translation(const float x, const float y, const float z)
	{
		return Matrix4x4(
			1.0f, 0.0f, 0.0f, x,
			0.0f, 1.0f, 0.0f, y,
			0.0f, 0.0f, 1.0f, z,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	constexpr Matrix4x4 Matrix4x4::Scaling(const float x, const float y, const float z)
	{
		return Matrix4x4(
			x, 0.0f, 0.0f, 0.0f,
			0.0f, y, 0.0f, 0.0f,
			0.0f, 0.0f, z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	constexpr Matrix4x4 Matrix4x4::Orthographic(const FloatBoundary& boundary, const float near, const float far)
	{
		const float l = boundary.Left;
		const float r = boundary.Right();
		const float t = boundary.Top;
		const float b = boundary.Bottom();

		return Matrix4x4(
			2.0f / (r - l), 0.0f, 0.0f, -(r + l) / (r - l),
			0.0f, 2.0f / (t - b), 0.0f, -(t + b) / (t - b),
			0.0f, 0.0f, -2.0f / (far - near), -(far + near) / (far - near),
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	inline constexpr Matrix4x4 Matrix4x4::Identity;
}