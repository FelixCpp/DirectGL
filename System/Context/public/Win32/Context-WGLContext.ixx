// Project Name : Context
// File Name    : Context-WGLContext123.ixx
// Author       : Felix Busch
// Created Date : 2025/10/02

module;

#include <Windows.h>

#include <string_view>

export module System.Context:WGLContext;

import System.Window;

import :Context;

namespace System
{
	class WGLContext : public Context
	{
	public:

		static std::unique_ptr<WGLContext> Create(const ContextSettings& settings);

		~WGLContext() override;

		void SetVerticalSyncEnabled(bool enabled) override;
		void Flush() override;

	private:

		explicit WGLContext(HDC deviceContext, HGLRC renderingContext, NativeWindowHandle parentHandle, const std::function<void(std::string_view)>& onError);

		void LogError(std::string_view message) const;

		HDC m_DeviceContext;
		HGLRC m_RenderingContext;
		NativeWindowHandle m_ParentHandle;

		std::function<void(std::string_view)> m_OnError;

	};
}