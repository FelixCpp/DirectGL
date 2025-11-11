module;

#include <glad/gl.h>
#include <ranges>

module DirectGL;
import DirectGL.ControlFlow;

import :MeshRenderer;

namespace DGL
{
	GLenum BlendFactorToGlId(const BlendMode::Factor factor)
	{
		switch (factor)
		{
			case BlendMode::Factor::Zero: return GL_ZERO;
			case BlendMode::Factor::One: return GL_ONE;
			case BlendMode::Factor::SrcColor: return GL_SRC_COLOR;
			case BlendMode::Factor::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
			case BlendMode::Factor::DstColor: return GL_DST_COLOR;
			case BlendMode::Factor::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
			case BlendMode::Factor::SrcAlpha: return GL_SRC_ALPHA;
			case BlendMode::Factor::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
			case BlendMode::Factor::DstAlpha: return GL_DST_ALPHA;
			case BlendMode::Factor::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
			case BlendMode::Factor::ConstantColor: return GL_CONSTANT_COLOR;
			case BlendMode::Factor::OneMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;
			case BlendMode::Factor::ConstantAlpha: return GL_CONSTANT_ALPHA;
			case BlendMode::Factor::OneMinusConstantAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
			case BlendMode::Factor::SrcAlphaSaturate: return GL_SRC_ALPHA_SATURATE;
			default: ThrowError("Unknown BlendFactor");
		}
	}

	GLenum BlendEquationToGlId(const BlendMode::Equation equation)
	{
		switch (equation)
		{
			case BlendMode::Equation::Add: return GL_FUNC_ADD;
			case BlendMode::Equation::Subtract: return GL_FUNC_SUBTRACT;
			case BlendMode::Equation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
			case BlendMode::Equation::Min: return GL_MIN;
			case BlendMode::Equation::Max: return GL_MAX;
			default: ThrowError("Unknown BlendEquation");
		}
	}

	MeshRenderer::MeshRenderer()
	{
	}

	void MeshRenderer::BeginDraw(const Math::Matrix4x4& projectionViewMatrix)
	{
		m_VertexRenderer.BeginDraw(projectionViewMatrix);
	}

	void MeshRenderer::EndDraw()
	{
		m_VertexRenderer.EndDraw();
	}

	void MeshRenderer::Submit(const std::span<const Mesh>& meshes, const BlendMode& blendMode, const Math::Matrix4x4& modelMatrix)
	{
		for (const Mesh& mesh : meshes)
		{
			Submit(mesh, blendMode, modelMatrix);
		}
	}

	void MeshRenderer::Submit(const Mesh& mesh, const BlendMode& blendMode, const Math::Matrix4x4& modelMatrix)
	{
		glBlendFuncSeparate(
			BlendFactorToGlId(blendMode.SourceColorFactor),
			BlendFactorToGlId(blendMode.DestinationColorFactor),
			BlendFactorToGlId(blendMode.SourceAlphaFactor),
			BlendFactorToGlId(blendMode.DestinationAlphaFactor)
		);

		glBlendEquationSeparate(
			BlendEquationToGlId(blendMode.ColorEquation),
			BlendEquationToGlId(blendMode.AlphaEquation)
		);

		// For each mesh we need to gather the positions and colors,
		// transforming them into our RenderableMeshVertex format
		// and storing them in the staging buffer.
		// in order to upload them to the GPU for rendering later on.

		// After we've transformed the mesh, we can submit it to the VertexRenderer
		m_VertexRenderer.Submit(std::span(mesh.Positions), std::span(mesh.Colors), std::span(mesh.Indices), modelMatrix);
	}

}