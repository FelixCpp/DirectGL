module;

#include <memory>
#include <format>

module DirectGL.RendererAdapter;

import Preconditions;

import :ResourceFactory;
import :OpenGLResourceFactory;

namespace DGL::RendererAdapter
{
	std::unique_ptr<ResourceFactory> ResourceFactory::Create(const RendererAPI rendererApi)
	{
		switch (rendererApi)
		{
			case RendererAPI::None: System::TODO();
			case RendererAPI::OpenGL: return std::make_unique<OpenGLResourceFactory>();
			default: System::Error(std::format("Unknown RendererAPI: {}", rendererApi));
		}
	}

}