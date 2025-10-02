module;

#include <memory>

module System.Window;

#ifdef WINDOW_PLATFORM_WINDOWS
typedef System::Win32Window WindowImpl;
#else
#endif

namespace System
{
	std::unique_ptr<Window> CreateWindow(const CreateWindowProperties& properties)
	{
		return WindowImpl::Create(properties);
	}
}