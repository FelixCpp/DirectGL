// Project Name : DirectGL
// File Name    : DirectGL-RenderingProperties.ixx
// Author       : Felix Busch
// Created Date : 2025/11/18

export module DirectGL:RenderingProperties;

import :ClipRect;
import :BlendMode;
import :Shader;

namespace DGL
{
	struct RenderingProperties
	{
		ClipRect				ClippingRect;
		BlendMode				BlendMode;
		Math::Matrix4x4			ModelMatrix;
		Shader*					Shader;
	};
}
