module System.Context;

#ifdef CONTEXT_PLATFORM_WINDOWS
import :WGLContext;
using ContextImpl = System::WGLContext;
#else
// TODO(Felix): Add other platform implementations
#endif

namespace System
{
	std::unique_ptr<Context> CreateContext(const ContextSettings& settings)
	{
		return ContextImpl::Create(settings);
	};
}