// Project Name : DirectGL
// File Name    : DGL-BorderSide.ixx
// Author       : Felix Busch
// Created Date : 2025/09/29

export module DGL:BorderSide;

import Math;

export namespace DGL
{
	struct BorderSide
	{
		virtual ~BorderSide() = default;
		virtual Math::Float2 Transform(const Math::Float2& point, float thickness) const = 0;

		static const BorderSide Inside;
		static const BorderSide Center;
		static const BorderSide Outside;
	};
}