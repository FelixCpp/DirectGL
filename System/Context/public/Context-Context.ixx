module;

#include <memory>
#include <functional>
#include <string_view>

export module System.Context:Context;

import System.Window;

namespace System
{
	export struct Context
	{
		virtual ~Context() = default;

		virtual void SetVerticalSyncEnabled(bool enabled) = 0;
		virtual void Flush() = 0;
	};

	export struct ContextSettings
	{
		int MajorVersion;
		int MinorVersion;
		bool IsDebuggingContext;
		bool IsCompatibilityContext;
		NativeWindowHandle ParentWindow;

		std::function<void(std::string_view)> OnError;
	};

	export std::unique_ptr<Context> CreateContext(const ContextSettings& settings);
}