// Project Name : DirectGL
// File Name    : DGL-Framebuffer.ixx
// Author       : Felix Busch
// Created Date : 2025/09/29

module;

#include <cstdint>

export module DGL:Framebuffer;

export namespace DGL
{
	struct FramebufferProperties
	{
		uint32_t Width;
		uint32_t Height;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		virtual void Begin() = 0;
		virtual void End() = 0;
	};
}