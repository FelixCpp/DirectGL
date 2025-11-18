// Project Name : DirectGL
// File Name    : DirectGL-Renderer2D.ixx
// Author       : Felix Busch
// Created Date : 2025/11/11

module;

#include <vector>
#include <optional>
#include <memory>
#include <span>
#include <glad/gl.h>
#include <string_view>

export module DirectGL:Renderer2D;

import DirectGL.Math;

import :BlendMode;
import :StrokeCap;
import :TextAlignment;
import :Image2D;
import :ImageSampler;
import :Font;
import :Shader;
import :ClipRect;

namespace DGL
{
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
		ClipRect					ClipRect;
		BlendMode					BlendMode;
		const Image2D*				Texture;
		const ImageSampler*			TextureSampler;
		Shader*						Shader;
		bool 						IsTextCommand;
	};

	struct DrawListItem
	{
		const Image2D*		Texture;
		const ImageSampler*	TextureSampler;
		Shader*				Shader;
		size_t				IndexStart;
		size_t				IndexCount;
		ClipRect			ClippingRect;
		BlendMode			BlendMode;
		bool				IsTextCommand;
	};

	struct OutlineDefinition
	{
		std::vector<Math::Float3>	Positions;
		std::vector<Math::Float4>	Colors;
		std::vector<uint32_t>		Indices;
	};

	struct RenderingProperties
	{
		ClipRect				ClippingRect;
		BlendMode				BlendMode;
		Math::Matrix4x4			ModelMatrix;
		Shader*					Shader;
	};

	class Renderer2D
	{
	public:

		Renderer2D();
		~Renderer2D();

		void SetViewport(const Math::FloatBoundary& viewport);

		void BeginFrame();
		void EndFrame();

		void FillRectangle(const Math::FloatBoundary& boundary, const Math::Float4& color, const RenderingProperties& properties);
		void DrawRectangle(const Math::FloatBoundary& boundary, const Math::Float4& color, float strokeWeight, const RenderingProperties& properties);

		void FillRoundedRectangle(const Math::FloatBoundary& boundary, const Math::BorderRadius& borderRadius, const Math::Float4& color, size_t cornerSegments, const RenderingProperties& properties);
		void DrawRoundedRectangle(const Math::FloatBoundary& boundary, const Math::BorderRadius& borderRadius, float strokeWeight, const Math::Float4& color, size_t cornerSegments, const RenderingProperties& properties);

		void FillEllipse(const Math::Float2& center, const Math::Radius& radius, const Math::Float4& color, size_t segments, const RenderingProperties& properties);
		void DrawEllipse(const Math::Float2& center, const Math::Radius& radius, float strokeWeight, const Math::Float4& color, size_t segments, const RenderingProperties& properties);

		void FillTriangle(const Math::Float2& p1, const Math::Float2& p2, const Math::Float2& p3, const Math::Float4& color, const RenderingProperties& properties);
		void DrawTriangle(const Math::Float2& p1, const Math::Float2& p2, const Math::Float2& p3, float strokeWeight, const Math::Float4& color, const RenderingProperties& properties);

		void FillLine(const Math::Float2& p1, const Math::Float2& p2, float strokeWeight, const Math::Float4& color, StrokeCap strokeCap, size_t roundedStrokeCapSegments, const RenderingProperties& properties);
		void Text(std::string_view text, Font& font, const Math::Float2& position, float textSize, const Math::Float4& color, TextAlignment alignment, const RenderingProperties& properties);
		void Image(const Math::FloatBoundary& boundary, const Math::FloatBoundary& sourceRectangle, const Image2D& image, const ImageSampler* sampler, const Math::Float4& color, const RenderingProperties& properties);

	private:

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

		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Image2D> m_WhiteTexture;
		std::unique_ptr<ImageSampler> m_DefaultSampler;

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