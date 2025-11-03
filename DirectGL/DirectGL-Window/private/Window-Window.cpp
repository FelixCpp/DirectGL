module DirectGL.Window;

#ifdef WINDOW_PLATFORM_WINDOWS
import :Win32Window;
typedef DGL::Internal::Win32Window WindowImpl;
#else
#endif

namespace DGL
{
	std::unique_ptr<Window> CreateWindow(const CreateWindowProperties& properties)
	{
		return WindowImpl::Create(properties);
	}
}