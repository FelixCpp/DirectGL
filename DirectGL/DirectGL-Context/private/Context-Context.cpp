module DirectGL.Context;

#ifdef CONTEXT_PLATFORM_WINDOWS
import :WGLContext;
using ContextImpl = DGL::Internal::WGLContext;
#else
// TODO(Felix): Add other platform implementations
#endif

namespace DGL
{
	std::unique_ptr<Context> CreateContext(const ContextSettings& settings)
	{
		return ContextImpl::Create(settings);
	};
}