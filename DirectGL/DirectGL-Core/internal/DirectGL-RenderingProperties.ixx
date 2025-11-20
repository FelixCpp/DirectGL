// Project Name : DirectGL
// File Name    : DirectGL-RenderingProperties.ixx
// Author       : Felix Busch
// Created Date : 2025/11/18

export module DirectGL:RenderingProperties;

import :ClipRect;
import :BlendMode;
import :Shader;
import :StrokeJoin;
import :StrokeCap;
import :StrokeAlignment;

namespace DGL
{
	/// @brief Properties related to a rendering operation.
	struct RenderingProperties
	{
		ClipRect				ClippingRect;		//!< The clipping rectangle to use for this rendering operation.
		BlendMode				BlendMode;			//!< The blend mode to use for this rendering operation.
		Math::Matrix4x4			ModelMatrix;		//!< The model transformation matrix to use for this rendering operation.
		Shader*					Shader;				//!< The shader to use for this rendering operation.
		bool					IsFillEnabled;		//!< Whether to fill the shape or not.
		bool					IsStrokeEnabled;	//!< Whether to apply a stroke to the shape or not.
	};

	/// @brief Stroke properties for non-SDF rendering.
	struct StrokeProperties
	{
		float			MiterLimit;		//!< The miter limit for stroke joins.
		StrokeAlignment	Alignment;		//!< The stroke alignment.
		StrokeJoin		StrokeJoin;		//!< The stroke join style.
		StrokeCap		StrokeCap;		//!< The stroke cap style.
		bool			IsClosed;		//!< Whether the stroke is closed or open.
	};
}