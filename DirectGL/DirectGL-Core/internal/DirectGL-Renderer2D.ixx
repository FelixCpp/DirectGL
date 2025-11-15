// Project Name : DirectGL
// File Name    : DirectGL-Renderer2D.ixx
// Author       : Felix Busch
// Created Date : 2025/11/11

module;

#include <vector>
#include <optional>
#include <span>
#include <glad/gl.h>
#include <string_view>

export module DirectGL:Renderer2D;

import DirectGL.Math;

import :BlendMode;
import :StrokeCap;
import :Font;
import :TextAlignment;

namespace DGL
{

	typedef std::optional<Math::IntBoundary> ClipRect;

	constexpr ClipRect MakeClipRect(const Math::IntBoundary& boundary, const bool clipEnabled)
	{
		if (not clipEnabled)
		{
			return std::nullopt;
		}

		return boundary;
	}

	struct Vertex2D
	{
		Math::Float3 Position; //< X, Y, Z
		Math::Float2 TexCoord; //< U, V
		Math::Float4 Color;    //< R, G, B, A
	};

	struct DrawCommand2D
	{
		std::span<const Vertex2D>	Vertices;
		std::span<const uint32_t>	Indices;
		ClipRect					ClippingRect;
		BlendMode					BlendMode;
		uint32_t					TextureId;
		bool						IsText;
		bool						IsSdfText;
	};

	struct DrawListItem
	{
		uint32_t		TextureId;
		size_t			IndexCount;
		ClipRect		ClippingRect;
		BlendMode		BlendMode;
		bool			IsText;
		bool			IsSdfText;
	};

	struct OutlineDefinition
	{
		std::vector<Math::Float3>	Positions;
		std::vector<Math::Float4>	Colors;
		std::vector<uint32_t>		Indices;
	};

	class Renderer2D
	{
	public:

		Renderer2D();
		~Renderer2D();

		void SetViewport(const Math::FloatBoundary& viewport);

		void BeginFrame();
		void EndFrame();

		void FillRectangle(const Math::FloatBoundary& boundary, const Math::Float4& color, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode);
		void DrawRectangle(const Math::FloatBoundary& boundary, const Math::Float4& color, float strokeWeight, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode);

		void FillEllipse(const Math::Float2& center, const Math::Radius& radius, const Math::Float4& color, size_t segments, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode);
		void DrawEllipse(const Math::Float2& center, const Math::Radius& radius, float strokeWeight, const Math::Float4& color, size_t segments, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode);

		void FillTriangle(const Math::Float2& p1, const Math::Float2& p2, const Math::Float2& p3, const Math::Float4& color, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode);
		void DrawTriangle(const Math::Float2& p1, const Math::Float2& p2, const Math::Float2& p3, float strokeWeight, const Math::Float4& color, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode);

		void FillLine(const Math::Float2& p1, const Math::Float2& p2, float strokeWeight, const Math::Float4& color, StrokeCap strokeCap, size_t roundedStrokeCapSegments, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode);

		Math::Float2 DrawText(std::string_view text, const Font& font, float fontSize, const Math::Float2& position, TextAlignment textAlign, const Math::Float4& color, const ClipRect& clipRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode);

	private:

		void Texture(const Math::FloatBoundary& boundary, uint32_t textureId, const Math::Float4& color, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode);

		void AddDrawCommand(const DrawCommand2D& command);
		void Flush();

		static OutlineDefinition GenerateOutline(
			const std::span<const Math::Float2>& points,
			const std::span<const Math::Float4>& colors,
			float strokeWeight,
			float depth
		);

		uint32_t m_VertexArrayId;
		uint32_t m_VertexBufferId;
		uint32_t m_ElementBufferId;
		uint32_t m_ShaderProgramId;

		uint32_t m_WhiteTextureId;

		size_t m_VertexBufferCapacity;
		size_t m_ElementBufferCapacity;

		uint32_t m_ProjectionLocation;

		std::vector<DrawListItem>	m_DrawList;
		std::vector<Vertex2D>		m_Vertices;
		std::vector<uint32_t>		m_Indices;

		Math::FloatBoundary m_Viewport;
		Math::Matrix4x4 m_ProjectionMatrix;

		float m_Depth;
	};

}