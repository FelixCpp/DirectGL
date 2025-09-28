// Project Name : DirectGL
// File Name    : DGL-ExtendMode.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

export module DGL:ExtendMode;

export namespace DGL
{
	enum class ExtendMode
	{
		Clamp,
		Wrap,
		Mirror,
	};
}