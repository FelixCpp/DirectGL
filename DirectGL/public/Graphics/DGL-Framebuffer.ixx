// Project Name : DirectGL
// File Name    : DGL-Framebuffer.ixx
// Author       : Felix Busch
// Created Date : 2025/09/29

export module DGL:Framebuffer;

export namespace DGL
{
	struct Framebuffer
	{
		virtual ~Framebuffer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
	};
}