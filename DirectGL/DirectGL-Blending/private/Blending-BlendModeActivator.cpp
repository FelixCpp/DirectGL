module;

#include <Glad/gl.h>

module DirectGL.Blending;

import Preconditions;

namespace DGL::Blending
{
	[[nodiscard]] constexpr GLenum BlendFactorToGlId(const BlendMode::Factor factor)
	{
		switch (factor)
		{
			case BlendMode::Factor::Zero:					return GL_ZERO;
			case BlendMode::Factor::One:					return GL_ONE;
			case BlendMode::Factor::SrcColor:				return GL_SRC_COLOR;
			case BlendMode::Factor::OneMinusSrcColor:		return GL_ONE_MINUS_SRC_COLOR;
			case BlendMode::Factor::DstColor:				return GL_DST_COLOR;
			case BlendMode::Factor::OneMinusDstColor:		return GL_ONE_MINUS_DST_COLOR;
			case BlendMode::Factor::SrcAlpha:				return GL_SRC_ALPHA;
			case BlendMode::Factor::OneMinusSrcAlpha:		return GL_ONE_MINUS_SRC_ALPHA;
			case BlendMode::Factor::DstAlpha:				return GL_DST_ALPHA;
			case BlendMode::Factor::OneMinusDstAlpha:		return GL_ONE_MINUS_DST_ALPHA;
			case BlendMode::Factor::ConstantColor:			return GL_CONSTANT_COLOR;
			case BlendMode::Factor::OneMinusConstantColor:	return GL_ONE_MINUS_CONSTANT_COLOR;
			case BlendMode::Factor::ConstantAlpha:			return GL_CONSTANT_ALPHA;
			case BlendMode::Factor::OneMinusConstantAlpha:	return GL_ONE_MINUS_CONSTANT_ALPHA;
			case BlendMode::Factor::SrcAlphaSaturate:		return GL_SRC_ALPHA_SATURATE;
			default: System::Error("Unknown blend factor");
		}
	}

	[[nodiscard]] constexpr GLenum BlendEquationToGlId(const BlendMode::Equation equation)
	{
		switch (equation)
		{
			case BlendMode::Equation::Add:				return GL_FUNC_ADD;
			case BlendMode::Equation::Subtract:			return GL_FUNC_SUBTRACT;
			case BlendMode::Equation::ReverseSubtract:	return GL_FUNC_REVERSE_SUBTRACT;
			case BlendMode::Equation::Min:				return GL_MIN;
			case BlendMode::Equation::Max:				return GL_MAX;
			default: System::Error("Unknown blend equation");
		}
	}

	void DefaultBlendModeActivator::Activate(const BlendMode& blendMode)
	{
		// Enable blending if not already enabled
		GLint isBlendingEnabled = GL_FALSE;
		glGetIntegerv(GL_BLEND, &isBlendingEnabled);
		if (isBlendingEnabled == GL_FALSE)
		{
			glEnable(GL_BLEND);
		}

		glBlendFuncSeparate(
			BlendFactorToGlId(blendMode.SourceFactorRGB),
			BlendFactorToGlId(blendMode.DestinationFactorRGB),
			BlendFactorToGlId(blendMode.SourceFactorAlpha),
			BlendFactorToGlId(blendMode.DestinationFactorAlpha)
		);

		glBlendEquationSeparate(
			BlendEquationToGlId(blendMode.BlendEquationRGB),
			BlendEquationToGlId(blendMode.BlendEquationAlpha)
		);
	}
}

namespace DGL::Blending
{
	CachingBlendModeActivator::CachingBlendModeActivator(BlendModeActivator& activator):
		m_Activator(activator),
		m_CurrentBlendMode(BlendModes::Alpha),
		m_IsFirstActivation(true)
	{
	}

	void CachingBlendModeActivator::Activate(const BlendMode& blendMode)
	{
		if (m_IsFirstActivation or (m_CurrentBlendMode != blendMode))
		{
			m_Activator.Activate(blendMode);
			m_CurrentBlendMode = blendMode;
			m_IsFirstActivation = false;
		}
	}
}