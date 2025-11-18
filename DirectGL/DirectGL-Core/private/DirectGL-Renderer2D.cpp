module;

#include <glad/gl.h>
#include <cstddef>
#include <cmath>
#include <cstdio>
#include <utility>
#include <array>
#include <string_view>
#include <format>
#include <algorithm>
#include <unordered_map>

module DirectGL;

import DirectGL.ControlFlow;
import :Renderer2D;

static constexpr auto VERTEX_SOURCE = R"(
#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in vec4 a_Color;

layout (location = 0) out vec4 v_Color;
layout (location = 1) out vec2 v_TexCoord;

uniform mat4 u_ProjectionMatrix;

void main()
{
	gl_Position = u_ProjectionMatrix * vec4(a_Position, 1.0);
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
}
)";

static constexpr auto FRAMENT_SOURCE = R"(
#version 460 core

layout (location = 0) out vec4 o_FragColor;
layout (location = 0) in vec4 v_Color;
layout (location = 1) in vec2 v_TexCoord;

layout (binding = 0) uniform sampler2D u_Texture;

uniform int u_IsText;

void main()
{
	if (u_IsText == 1)
	{
		float alpha = texture(u_Texture, v_TexCoord).r;
		o_FragColor = vec4(v_Color.rgb, v_Color.a * alpha);
	} else {
		o_FragColor = texture(u_Texture, v_TexCoord) * v_Color;
	}
}
)";

namespace DGL
{
	constexpr GLenum BlendFactorToGlId(const BlendMode::Factor factor)
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
			default:										ThrowError("Unknown BlendMode::Factor");
		}
	}

	constexpr GLenum BlendEquationToGlId(const BlendMode::Equation equation)
	{
		switch (equation)
		{
			case BlendMode::Equation::Add:					return GL_FUNC_ADD;
			case BlendMode::Equation::Subtract:				return GL_FUNC_SUBTRACT;
			case BlendMode::Equation::ReverseSubtract:		return GL_FUNC_REVERSE_SUBTRACT;
			case BlendMode::Equation::Min:					return GL_MIN;
			case BlendMode::Equation::Max:					return GL_MAX;
			default:										ThrowError("Unknown BlendMode::Equation");
		}
	}

	Renderer2D::Renderer2D() :
		m_VertexArrayId(0),
		m_VertexBufferId(0),
		m_ElementBufferId(0),
		m_VertexBufferCapacity(65536),
		m_ElementBufferCapacity(131072),
		m_ProjectionLocation(0),
		m_Depth(0.0f)
	{
		glCreateBuffers(1, &m_VertexBufferId);
		glNamedBufferStorage(m_VertexBufferId, m_VertexBufferCapacity * sizeof(Vertex2D), nullptr, GL_DYNAMIC_STORAGE_BIT);

		glCreateBuffers(1, &m_ElementBufferId);
		glNamedBufferStorage(m_ElementBufferId, m_ElementBufferCapacity * sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);

		glCreateVertexArrays(1, &m_VertexArrayId);
		glVertexArrayVertexBuffer(m_VertexArrayId, 0, m_VertexBufferId, 0, sizeof(Vertex2D));
		glVertexArrayElementBuffer(m_VertexArrayId, m_ElementBufferId);

		// 2D Position (X, Y)
		glEnableVertexArrayAttrib(m_VertexArrayId, 0);
		glVertexArrayAttribFormat(m_VertexArrayId, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, Position));
		glVertexArrayAttribBinding(m_VertexArrayId, 0, 0);

		// 2D ImageSampler Coordinates (U, V)
		glEnableVertexArrayAttrib(m_VertexArrayId, 1);
		glVertexArrayAttribFormat(m_VertexArrayId, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, TexCoord));
		glVertexArrayAttribBinding(m_VertexArrayId, 1, 0);

		// 4D Color (R, G, B, A)
		glEnableVertexArrayAttrib(m_VertexArrayId, 2);
		glVertexArrayAttribFormat(m_VertexArrayId, 2, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, Color));
		glVertexArrayAttribBinding(m_VertexArrayId, 2, 0);

		m_Shader = Shader::CreateFromSource(VERTEX_SOURCE, FRAMENT_SOURCE);

		constexpr uint8_t whitePixel[4] = { 255, 255, 255, 255 };
		m_WhiteTexture = Image2D::CreateFromMemory(1, 1, whitePixel);
		m_DefaultSampler = ImageSampler::Create();
	}

	Renderer2D::~Renderer2D()
	{
		glDeleteBuffers(1, &m_VertexBufferId);
		glDeleteBuffers(1, &m_ElementBufferId);
		glDeleteVertexArrays(1, &m_VertexArrayId);
	}

	void Renderer2D::SetViewport(const Math::FloatBoundary& viewport)
	{
		m_Viewport = viewport;
		m_ProjectionMatrix = Math::Matrix4x4::Orthographic(viewport, -1.0f, 1.0f);
	}

	void Renderer2D::BeginFrame()
	{
		m_Depth = 0.0f;

		//glClear(GL_DEPTH_BUFFER_BIT);

		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer2D::EndFrame()
	{
		Flush();
	}

	void Renderer2D::FillRectangle(const Math::FloatBoundary& boundary, const Math::Float4& color, const RenderingProperties& properties)
	{
		const auto& [clippingRect, blendMode, modelMatrix, shader] = properties;
		const auto [left, top, width, height] = boundary;

		const std::array corners = {
			modelMatrix.TransformPoint(Math::Float2{ left,         top          }),
			modelMatrix.TransformPoint(Math::Float2{ left + width, top          }),
			modelMatrix.TransformPoint(Math::Float2{ left + width, top + height }),
			modelMatrix.TransformPoint(Math::Float2{ left,         top + height }),
		};

		const Vertex2D vertices[] = {
			Vertex2D{ .Position = Math::Float3{ corners[0], m_Depth },	.Color = color },
			Vertex2D{ .Position = Math::Float3{ corners[1], m_Depth },	.Color = color },
			Vertex2D{ .Position = Math::Float3{ corners[2], m_Depth },	.Color = color },
			Vertex2D{ .Position = Math::Float3{ corners[3], m_Depth },	.Color = color },
		};

		const uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		AddDrawCommand(DrawCommand2D {
			.Vertices = std::span(vertices),
			.Indices = std::span(indices),
			.ClipRect = clippingRect,
			.BlendMode = blendMode,
			.Texture = m_WhiteTexture.get(),
			.TextureSampler = m_DefaultSampler.get(),
			.Shader = shader != nullptr ? shader : m_Shader.get(),
			.IsTextCommand = false,
		});
	}

	void Renderer2D::DrawRectangle(const Math::FloatBoundary& boundary, const Math::Float4& color, const float strokeWeight, const RenderingProperties& properties)
	{
		const auto& [clippingRect, blendMode, modelMatrix, shader] = properties;
		const auto [left, top, width, height] = boundary;

		const std::array corners = {
			modelMatrix.TransformPoint(Math::Float2{ left,         top          }),
			modelMatrix.TransformPoint(Math::Float2{ left + width, top          }),
			modelMatrix.TransformPoint(Math::Float2{ left + width, top + height }),
			modelMatrix.TransformPoint(Math::Float2{ left,         top + height }),
		};

		const std::array colors = { color, color, color, color };
		const OutlineDefinition outline = GenerateOutline(std::span{ corners }, std::span{ colors }, strokeWeight, m_Depth);

		std::vector<Vertex2D> vertexData;
		vertexData.reserve(outline.Positions.size());
		for (size_t i = 0; i < outline.Positions.size(); ++i)
		{
			vertexData.push_back(Vertex2D{
				.Position = outline.Positions[i],
				.Color = outline.Colors[i],
			});
		}

		AddDrawCommand(DrawCommand2D {
			.Vertices = std::span(vertexData),
			.Indices = std::span(outline.Indices.data(), outline.Indices.size()),
			.ClipRect = clippingRect,
			.BlendMode = blendMode,
			.Texture = m_WhiteTexture.get(),
			.TextureSampler = m_DefaultSampler.get(),
			.Shader = shader != nullptr ? shader : m_Shader.get(),
			.IsTextCommand = false,
		});
	}

	void Renderer2D::FillRoundedRectangle(const Math::FloatBoundary& boundary, const Math::BorderRadius& borderRadius, const Math::Float4& color, const size_t cornerSegments, const RenderingProperties& properties)
	{
		const auto& [clippingRect, blendMode, modelMatrix, shader] = properties;
		const auto generateEllipseCorner = [](const Math::Float2& center, const Math::Angle startAngle, const Math::Angle stopAngle, const float& radius, uint32_t segments) -> std::vector<Math::Float2>
		{
			std::vector<Math::Float2> positions;
			const Math::Angle angleRange = stopAngle - startAngle;

			for (size_t i = 0; i <= segments; ++i)
			{
				const float angle = startAngle.AsRadians() + (static_cast<float>(i) / static_cast<float>(segments)) * angleRange.AsRadians();
				const float x = center.X + std::cos(angle) * radius;
				const float y = center.Y + std::sin(angle) * radius;
				positions.emplace_back(x, y);
			}

			return positions;
		};

		const Math::Float2 topLeftCenter = { boundary.Left + borderRadius.TopLeft, boundary.Top + borderRadius.TopLeft };
		const Math::Float2 topRightCenter = { boundary.Left + boundary.Width - borderRadius.TopRight, boundary.Top + borderRadius.TopRight };
		const Math::Float2 bottomRightCenter = { boundary.Left + boundary.Width - borderRadius.BottomRight, boundary.Top + boundary.Height - borderRadius.BottomRight };
		const Math::Float2 bottomLeftCenter = { boundary.Left + borderRadius.BottomLeft, boundary.Top + boundary.Height - borderRadius.BottomLeft };

		const std::vector<Math::Float2> topLeftPoints = generateEllipseCorner(topLeftCenter, Math::Degrees(180.0f), Math::Degrees(270.0f), borderRadius.TopLeft, cornerSegments);
		const std::vector<Math::Float2> topRightPoints = generateEllipseCorner(topRightCenter, Math::Degrees(270.0f), Math::Degrees(360.0f), borderRadius.TopRight, cornerSegments);
		const std::vector<Math::Float2> bottomRightPoints = generateEllipseCorner(bottomRightCenter, Math::Degrees(0.0f), Math::Degrees(90.0f), borderRadius.BottomRight, cornerSegments);
		const std::vector<Math::Float2> bottomLeftPoints = generateEllipseCorner(bottomLeftCenter, Math::Degrees(90.0f), Math::Degrees(180.0f), borderRadius.BottomLeft, cornerSegments);

		std::vector<Vertex2D> vertices;
		std::vector<uint32_t> indices;

		// Insert the center vertex
		{
			const Math::Float2 transformedCenter = modelMatrix.TransformPoint(boundary.Center());
			vertices.push_back(Vertex2D{ .Position = Math::Float3{ transformedCenter.X, transformedCenter.Y, m_Depth }, .Color = color });
		}

		// Insert top left corner points and indices
		for (size_t i = 0; i < topLeftPoints.size(); ++i)
		{
			const Math::Float2 transformedPoint = modelMatrix.TransformPoint(topLeftPoints[i]);
			vertices.push_back(Vertex2D{ .Position = Math::Float3{ transformedPoint.X, transformedPoint.Y, m_Depth }, .Color = color });

			if (i < topLeftPoints.size() - 1)
			{
				indices.emplace_back(0);
				indices.emplace_back(i + 1);
				indices.emplace_back(i + 2);
			}
		}

		// Insert top right corner points and indices
		const size_t topRightStartIndex = vertices.size();
		for (size_t i = 0; i < topRightPoints.size(); ++i)
		{
			const Math::Float2 transformedPoint = modelMatrix.TransformPoint(topRightPoints[i]);
			vertices.push_back(Vertex2D{ .Position = Math::Float3{ transformedPoint.X, transformedPoint.Y, m_Depth }, .Color = color });

			if (i < topRightPoints.size() - 1)
			{
				indices.emplace_back(0);
				indices.emplace_back(topRightStartIndex + i);
				indices.emplace_back(topRightStartIndex + i + 1);
			}
		}

		// Connect top left to top right
		indices.emplace_back(0);
		indices.emplace_back(topLeftPoints.size());
		indices.emplace_back(topRightStartIndex);

		// Insert bottom right corner points and indices
		const size_t bottomRightStartIndex = vertices.size();
		for (size_t i = 0; i < bottomRightPoints.size(); ++i)
		{
			const Math::Float2 transformedPoint = modelMatrix.TransformPoint(bottomRightPoints[i]);
			vertices.push_back(Vertex2D{ .Position = Math::Float3{ transformedPoint.X, transformedPoint.Y, m_Depth }, .Color = color });
			if (i < bottomRightPoints.size() - 1)
			{
				indices.emplace_back(0);
				indices.emplace_back(bottomRightStartIndex + i);
				indices.emplace_back(bottomRightStartIndex + i + 1);
			}
		}

		// Connect top right to bottom right
		indices.emplace_back(0);
		indices.emplace_back(topRightStartIndex + topRightPoints.size() - 1);
		indices.emplace_back(bottomRightStartIndex);

		// Insert bottom left corner points and indices
		const size_t bottomLeftStartIndex = vertices.size();
		for (size_t i = 0; i < bottomLeftPoints.size(); ++i)
		{
			const Math::Float2 transformedPoint = modelMatrix.TransformPoint(bottomLeftPoints[i]);
			vertices.push_back(Vertex2D{ .Position = Math::Float3{ transformedPoint.X, transformedPoint.Y, m_Depth }, .Color = color });
			if (i < bottomLeftPoints.size() - 1)
			{
				indices.emplace_back(0);
				indices.emplace_back(bottomLeftStartIndex + i);
				indices.emplace_back(bottomLeftStartIndex + i + 1);
			}
		}

		// Connect bottom right to bottom left
		indices.emplace_back(0);
		indices.emplace_back(bottomRightStartIndex + bottomRightPoints.size() - 1);
		indices.emplace_back(bottomLeftStartIndex);

		// Connect bottom left to top left
		indices.emplace_back(0);
		indices.emplace_back(bottomLeftStartIndex + bottomLeftPoints.size() - 1);
		indices.emplace_back(1);

		AddDrawCommand(DrawCommand2D {
			.Vertices = std::span(vertices),
			.Indices = std::span(indices),
			.ClipRect = clippingRect,
			.BlendMode = blendMode,
			.Texture = m_WhiteTexture.get(),
			.TextureSampler = m_DefaultSampler.get(),
			.Shader = shader != nullptr ? shader : m_Shader.get(),
			.IsTextCommand = false,
		});
	}

	void Renderer2D::DrawRoundedRectangle(const Math::FloatBoundary& boundary, const Math::BorderRadius& borderRadius, float strokeWeight, const Math::Float4& color, size_t cornerSegments, const RenderingProperties& properties)
	{
		
	}


	void Renderer2D::FillEllipse(const Math::Float2& center, const Math::Radius& radius, const Math::Float4& color, const size_t segments, const RenderingProperties& properties)
	{
		const auto& [clippingRect, blendMode, modelMatrix, shader] = properties;
		const Math::Float2 transformedCenter = modelMatrix.TransformPoint(center);
		const Math::Float2 transformedRadius = modelMatrix.TransformVector(Math::Float2{ radius.X, radius.Y });

		std::vector<Vertex2D> vertices;
		vertices.reserve(segments + 1);

		// Center vertex
		vertices.push_back(Vertex2D{ .Position = Math::Float3{ transformedCenter.X, transformedCenter.Y, m_Depth }, .Color = color });

		// Perimeter vertices
		for (uint32_t i = 0; i <= segments; ++i)
		{
			const float angle = (static_cast<float>(i) / static_cast<float>(segments)) * Math::TAU;
			const float x = transformedCenter.X + std::cos(angle) * transformedRadius.X;
			const float y = transformedCenter.Y + std::sin(angle) * transformedRadius.Y;
			vertices.push_back(Vertex2D{ .Position = Math::Float3{ x, y, m_Depth }, .Color = color });
		}

		std::vector<uint32_t> indices;
		indices.reserve(segments * 3);

		for (uint32_t i = 1; i <= segments; ++i)
		{
			indices.push_back(0);        // Center vertex
			indices.push_back(i);        // Current perimeter vertex
			indices.push_back(i + 1);    // Next perimeter vertex
		}

		AddDrawCommand(DrawCommand2D {
			.Vertices = std::span(vertices),
			.Indices = std::span(indices),
			.ClipRect = clippingRect,
			.BlendMode = blendMode,
			.Texture = m_WhiteTexture.get(),
			.TextureSampler = m_DefaultSampler.get(),
			.Shader = shader != nullptr ? shader : m_Shader.get(),
			.IsTextCommand = false,
		});
	}

	void Renderer2D::DrawEllipse(const Math::Float2& center, const Math::Radius& radius, const float strokeWeight, const Math::Float4& color, size_t segments, const RenderingProperties& properties)
	{
		const auto& [clippingRect, blendMode, modelMatrix, shader] = properties;
		const Math::Float2 transformedCenter = modelMatrix.TransformPoint(center);
		const Math::Float2 transformedRadius = modelMatrix.TransformVector(Math::Float2{ radius.X, radius.Y });

		std::vector<Math::Float2> circlePoints(segments);
		std::vector<Math::Float4> circleColors(segments);
		for (size_t i = 0; i < segments; ++i)
		{
			const float angle = (static_cast<float>(i) / static_cast<float>(segments)) * Math::TAU;
			const float x = transformedCenter.X + std::cos(angle) * transformedRadius.X;
			const float y = transformedCenter.Y + std::sin(angle) * transformedRadius.Y;
			circlePoints[i] = Math::Float2{ x, y };
			circleColors[i] = color;
		}

		const OutlineDefinition outline = GenerateOutline(
			std::span(circlePoints),
			std::span(circleColors),
			strokeWeight,
			m_Depth
		);

		std::vector<Vertex2D> vertices;
		vertices.reserve(outline.Positions.size());
		for (size_t i = 0; i < outline.Positions.size(); ++i)
		{
			vertices.push_back(Vertex2D {
				.Position = outline.Positions[i],
				.Color = outline.Colors[i],
			});
		}

		AddDrawCommand(DrawCommand2D {
			.Vertices = std::span(vertices),
			.Indices = std::span(outline.Indices),
			.ClipRect = clippingRect,
			.BlendMode = blendMode,
			.Texture = m_WhiteTexture.get(),
			.TextureSampler = m_DefaultSampler.get(),
			.Shader = shader != nullptr ? shader : m_Shader.get(),
			.IsTextCommand = false,
		});
	}

	void Renderer2D::FillTriangle(const Math::Float2& p1, const Math::Float2& p2, const Math::Float2& p3, const Math::Float4& color, const RenderingProperties& properties)
	{
		const auto& [clippingRect, blendMode, modelMatrix, shader] = properties;
		const Math::Float2 tp1 = modelMatrix.TransformPoint(p1);
		const Math::Float2 tp2 = modelMatrix.TransformPoint(p2);
		const Math::Float2 tp3 = modelMatrix.TransformPoint(p3);

		const Vertex2D vertices[] = {
			Vertex2D{ .Position = Math::Float3{ tp1.X, tp1.Y, m_Depth }, .Color = color },
			Vertex2D{ .Position = Math::Float3{ tp2.X, tp2.Y, m_Depth }, .Color = color },
			Vertex2D{ .Position = Math::Float3{ tp3.X, tp3.Y, m_Depth }, .Color = color },
		};

		constexpr uint32_t indices[] = { 0, 1, 2 };

		AddDrawCommand(DrawCommand2D {
			.Vertices = std::span(vertices),
			.Indices = std::span(indices),
			.ClipRect = clippingRect,
			.BlendMode = blendMode,
			.Texture = m_WhiteTexture.get(),
			.TextureSampler = m_DefaultSampler.get(),
			.Shader = shader != nullptr ? shader : m_Shader.get(),
			.IsTextCommand = false,
		});
	}

	void Renderer2D::DrawTriangle(const Math::Float2& p1, const Math::Float2& p2, const Math::Float2& p3, const float strokeWeight, const Math::Float4& color, const RenderingProperties& properties)
	{
		const auto& [clippingRect, blendMode, modelMatrix, shader] = properties;
		const std::array points = {
			modelMatrix.TransformPoint(p1),
			modelMatrix.TransformPoint(p2),
			modelMatrix.TransformPoint(p3),
		};

		const std::array colors = { color, color, color, };
		const OutlineDefinition outline = GenerateOutline(
			std::span(points),
			std::span(colors),
			strokeWeight,
			m_Depth
		);

		std::vector<Vertex2D> vertices;
		vertices.reserve(outline.Positions.size());
		for (size_t i = 0; i < outline.Positions.size(); ++i)
		{
			vertices.push_back(Vertex2D {
				.Position = outline.Positions[i],
				.Color = outline.Colors[i],
			});
		}

		AddDrawCommand(DrawCommand2D {
			.Vertices = std::span(vertices),
			.Indices = std::span(outline.Indices),
			.ClipRect = clippingRect,
			.BlendMode = blendMode,
			.Texture = m_WhiteTexture.get(),
			.TextureSampler = m_DefaultSampler.get(),
			.Shader = shader != nullptr ? shader : m_Shader.get(),
			.IsTextCommand = false,
		});
	}

	void InsertButtLineCap(std::vector<Math::Float3>& positions, std::vector<Math::Float4>& colors, const Math::Float2& point, const Math::Float4& color, const Math::Float2& offset, const float depth)
	{
		// Add the positions
		positions.emplace_back(point.X + offset.X, point.Y + offset.Y, depth);
		positions.emplace_back(point.X - offset.X, point.Y - offset.Y, depth);

		// Add the colors
		colors.emplace_back(color);
		colors.emplace_back(color);
	}

	void InsertSquareLineCap(std::vector<Math::Float3>& positions, std::vector<Math::Float4>& colors, const Math::Float2& point, const Math::Float4& color, const Math::Float2& direction, const Math::Float2& offset, const float depth)
	{
		const Math::Float2 capOffset = direction * offset.Length();

		// Add the positions
		positions.emplace_back(point.X + offset.X + capOffset.X, point.Y + offset.Y + capOffset.Y, depth);
		positions.emplace_back(point.X - offset.X + capOffset.X, point.Y - offset.Y + capOffset.Y, depth);

		// Add the colors
		colors.emplace_back(color);
		colors.emplace_back(color);
	}

	void InsertRoundLineCap(std::vector<Math::Float3>& positions, std::vector<Math::Float4>& colors, const Math::Float2& point, const Math::Float4& color, const Math::Angle startAngle, const Math::Angle sweepAngle, const float radius, const uint32_t segments, const float depth)
	{
		const float startRadians = startAngle.AsRadians();
		const float sweepRadians = sweepAngle.AsRadians();

		// Compute the angle step size per segment
		const float angleStepSize = sweepRadians / static_cast<float>(segments);

		for (uint32_t i = 0; i <= segments; ++i)
		{
			const float angle = startRadians + angleStepSize * static_cast<float>(i);
			const float x = point.X + std::cos(angle) * radius;
			const float y = point.Y + std::sin(angle) * radius;

			positions.emplace_back(x, y, depth);
			colors.emplace_back(color);
		}
	}

	void Renderer2D::FillLine(const Math::Float2& p1, const Math::Float2& p2, const float strokeWeight, const Math::Float4& color, StrokeCap strokeCap, size_t roundedStrokeCapSegments, const RenderingProperties& properties)
	{
		const auto& [clippingRect, blendMode, modelMatrix, shader] = properties;
		const std::array points = { modelMatrix.TransformPoint(p1), modelMatrix.TransformPoint(p2) };
		const std::array colors = { color, color };

		// Make some optimizations for degenerate lines
		if (strokeWeight < 3.0f)
		{
			strokeCap = StrokeCap::Butt;
		}

		// Compute the direction vector of the line
		const float halfStrokeWeight = strokeWeight * 0.5f;
		const Math::Float2 direction = (points[1] - points[0]).Normalized();
		const Math::Float2 offset = direction.Perpendicular() * halfStrokeWeight;
		const Math::Angle startAngle = direction.Heading() + Math::Degrees(90.0f);

		std::vector<Math::Float3> positions;
		std::vector<Math::Float4> meshColors;

		// Insert the start cap
		switch (strokeCap.Start)
		{
			case StrokeCapType::Butt: InsertButtLineCap(positions, meshColors, points[0], colors[0], offset, m_Depth); break;
			case StrokeCapType::Square: InsertSquareLineCap(positions, meshColors, points[0], colors[0], -direction, offset, m_Depth); break;
			case StrokeCapType::Round: InsertRoundLineCap(positions, meshColors, points[0], colors[0], startAngle, Math::Degrees(180.0f), halfStrokeWeight, roundedStrokeCapSegments, m_Depth); break;
			default: ThrowError("Unknown StrokeCap in MeshBuilder::GenerateLineMesh()");
		}

		// Store the number of positions used for the start cap
		const size_t startPositionCount = positions.size();

		// Insert the end cap
		switch (strokeCap.End)
		{
			case StrokeCapType::Butt: InsertButtLineCap(positions, meshColors, points[1], colors[1], offset, m_Depth); break;
			case StrokeCapType::Square: InsertSquareLineCap(positions, meshColors, points[1], colors[1], direction, offset, m_Depth); break;
			case StrokeCapType::Round: InsertRoundLineCap(positions, meshColors, points[1], colors[1], startAngle, Math::Degrees(-180.0f), halfStrokeWeight, roundedStrokeCapSegments, m_Depth); break;
			default: ThrowError("Unknown StrokeCap in MeshBuilder::GenerateLineMesh()");
		}

		// Store the number of positions used for the end cap
		const size_t endPositionCount = positions.size() - startPositionCount;

		// Now we need to create the indices for the line mesh.
		std::vector<uint32_t> indices;

		// The indices are going to be connected like a triangle strip between the start and end caps.
		for (size_t i = 0; i < std::max(startPositionCount, endPositionCount); ++i)
		{
			const size_t currentStartIndex = std::min(i, startPositionCount - 1);
			const size_t nextStartIndex = std::min(i + 1, startPositionCount - 1);
			const size_t currentEndIndex = startPositionCount + std::min(i, endPositionCount - 1);
			const size_t nextEndIndex = startPositionCount + std::min(i + 1, endPositionCount - 1);

			// First triangle
			indices.emplace_back(static_cast<uint32_t>(currentStartIndex));
			indices.emplace_back(static_cast<uint32_t>(currentEndIndex));
			indices.emplace_back(static_cast<uint32_t>(nextStartIndex));

			// Second triangle
			indices.emplace_back(static_cast<uint32_t>(nextStartIndex));
			indices.emplace_back(static_cast<uint32_t>(currentEndIndex));
			indices.emplace_back(static_cast<uint32_t>(nextEndIndex));
		}

		std::vector<Vertex2D> vertexData;
		vertexData.reserve(positions.size());
		for (size_t i = 0; i < positions.size(); ++i)
		{
			vertexData.push_back(Vertex2D{
				.Position = positions[i],
				.Color = meshColors[i],
			});
		}

		AddDrawCommand(DrawCommand2D {
			.Vertices = std::span(vertexData),
			.Indices = std::span(indices),
			.ClipRect = clippingRect,
			.BlendMode = blendMode,
			.Texture = m_WhiteTexture.get(),
			.TextureSampler = m_DefaultSampler.get(),
			.Shader = shader != nullptr ? shader : m_Shader.get(),
			.IsTextCommand = false,
		});
	}

	struct GlyphRenderInfo
	{
		std::vector<Vertex2D> vertices;
		std::vector<uint32_t> indices;
		const Image2D* fontAtlas;
	};

	void Renderer2D::Text(const std::string_view text, Font& font, const Math::Float2& position, const float textSize, const Math::Float4& color, TextAlignment alignment, const RenderingProperties& properties)
	{
		const auto& [clippingRect, blendMode, modelMatrix, shader] = properties;
		const float scale = textSize / static_cast<float>(font.GetTextSize());
		const float lineHeight = font.GetLineHeight() * scale;

		std::unordered_map<size_t, GlyphRenderInfo> renderInfos;

		Math::Float2 cursorPosition = position;
		for (size_t i = 0; i < text.length(); ++i)
		{
			const char32_t character = text[i];

			if (character == '\n')
			{
				cursorPosition.X = position.X;
				cursorPosition.Y += lineHeight;
				continue;
			}

			if (character == '\t')
			{
				if (const Glyph* glyph = font.GetGlyph(' '))
				{
					cursorPosition.X += glyph->Advance * scale * 4.0f;
					continue;
				}
			}

			const Glyph* glyph = font.GetGlyph(character);
			if (glyph == nullptr)
			{
				continue;
			}

			if (character == ' ')
			{
				cursorPosition.X += glyph->Advance * scale;
				continue;
			}

			GlyphRenderInfo& renderInfo = renderInfos[glyph->AtlasIndex];
			renderInfo.fontAtlas = font.GetPage(glyph->AtlasIndex).TextureAtlas.get();

			const auto [glyphUVLeft, glyphUVTop, glyphUVWidth, glyphUVHeight] = glyph->AtlasRectUV;
			const float glyphUVRight = glyphUVLeft + glyphUVWidth;
			const float glyphUVBottom = glyphUVTop + glyphUVHeight;

			const float glyphLeft = cursorPosition.X + glyph->Bearing.X * scale;
			const float glyphTop = cursorPosition.Y - (glyph->Bearing.Y) * scale;
			const float glyphRight = glyphLeft + glyph->AtlasRect.Width * scale;
			const float glyphBottom = glyphTop + glyph->AtlasRect.Height * scale;

			renderInfo.vertices.append_range(std::initializer_list{
				Vertex2D{ .Position = modelMatrix.TransformPoint({ glyphLeft, glyphTop, m_Depth }), .TexCoord = { glyphUVLeft, glyphUVTop }, .Color = color },
				Vertex2D{ .Position = modelMatrix.TransformPoint({ glyphRight, glyphTop, m_Depth }), .TexCoord = { glyphUVRight, glyphUVTop }, .Color = color },
				Vertex2D{ .Position = modelMatrix.TransformPoint({ glyphRight, glyphBottom, m_Depth }), .TexCoord = { glyphUVRight, glyphUVBottom }, .Color = color },
				Vertex2D{ .Position = modelMatrix.TransformPoint({ glyphLeft, glyphBottom, m_Depth }), .TexCoord = { glyphUVLeft, glyphUVBottom }, .Color = color },
			});

			renderInfo.indices.append_range(std::initializer_list{
				static_cast<uint32_t>(renderInfo.vertices.size() - 4),
				static_cast<uint32_t>(renderInfo.vertices.size() - 3),
				static_cast<uint32_t>(renderInfo.vertices.size() - 2),
				static_cast<uint32_t>(renderInfo.vertices.size() - 2),
				static_cast<uint32_t>(renderInfo.vertices.size() - 1),
				static_cast<uint32_t>(renderInfo.vertices.size() - 4),
			});

			cursorPosition.X += glyph->Advance * scale;
		}

		for (const auto& [atlasIndex, renderInfo] : renderInfos)
		{
			AddDrawCommand(DrawCommand2D {
				.Vertices = std::span(renderInfo.vertices),
				.Indices = std::span(renderInfo.indices),
				.ClipRect = clippingRect,
				.BlendMode = blendMode,
				.Texture = renderInfo.fontAtlas,
				.TextureSampler = m_DefaultSampler.get(),
				.Shader = shader != nullptr ? shader : m_Shader.get(),
				.IsTextCommand = true,
			});
		}
	}

	void Renderer2D::Image(const Math::FloatBoundary& boundary, const Math::FloatBoundary& sourceRectangle, const Image2D& image, const ImageSampler* sampler, const Math::Float4& color, const RenderingProperties& properties)
	{
		const auto& [clippingRect, blendMode, modelMatrix, shader] = properties;
		const auto [left, top, width, height] = boundary;
		const std::array corners = {
			modelMatrix.TransformPoint(Math::Float2{ left,         top          }),
			modelMatrix.TransformPoint(Math::Float2{ left + width, top          }),
			modelMatrix.TransformPoint(Math::Float2{ left + width, top + height }),
			modelMatrix.TransformPoint(Math::Float2{ left,         top + height }),
		};

		const auto imageSize = static_cast<Math::Float2>(image.GetSize());

		// Convert source rectangle to UV coordinates
		const auto [srcLeft, srcTop, srcWidth, srcHeight] = sourceRectangle;
		const float u0 = srcLeft / imageSize.X;
		const float v0 = srcTop / imageSize.Y;
		const float u1 = (srcLeft + srcWidth) / imageSize.X;
		const float v1 = (srcTop + srcHeight) / imageSize.Y;

		const Vertex2D vertices[] = {
			Vertex2D{ .Position = Math::Float3{ corners[0], m_Depth },	.TexCoord = Math::Float2{ u0, v0 },	.Color = color },
			Vertex2D{ .Position = Math::Float3{ corners[1], m_Depth },	.TexCoord = Math::Float2{ u1, v0 },	.Color = color },
			Vertex2D{ .Position = Math::Float3{ corners[2], m_Depth },	.TexCoord = Math::Float2{ u1, v1 },	.Color = color },
			Vertex2D{ .Position = Math::Float3{ corners[3], m_Depth },	.TexCoord = Math::Float2{ u0, v1 },	.Color = color },
		};

		constexpr uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		AddDrawCommand(DrawCommand2D {
			.Vertices = std::span(vertices),
			.Indices = std::span(indices),
			.ClipRect = clippingRect,
			.BlendMode = blendMode,
			.Texture = &image,
			.TextureSampler = sampler,
			.Shader = shader != nullptr ? shader : m_Shader.get(),
			.IsTextCommand = false,
		});
	}

	void Renderer2D::AddDrawCommand(const DrawCommand2D& command)
	{
		// Check if we need to flush the current draw list
		if (m_Vertices.size() + command.Vertices.size() > m_VertexBufferCapacity or m_Indices.size() + command.Indices.size() > m_ElementBufferCapacity)
		{
			Flush();
		}

		const size_t verticesOffset = m_Vertices.size();
		const size_t indicesOffset = m_Indices.size();

		// Append the vertices
		m_Vertices.insert(m_Vertices.end(), command.Vertices.begin(), command.Vertices.end());

		// Append the indices
		for (const uint32_t index : command.Indices) m_Indices.push_back(index + verticesOffset);

		// Flip Y for clipping rect
		ClipRect flippedClipRect = command.ClipRect;
		if (flippedClipRect.IsClipped())
		{
			Math::IntBoundary& flippedBoundary = flippedClipRect.UnwrapBoundary();
			flippedBoundary.Top = static_cast<int>(m_Viewport.Height) - (flippedBoundary.Top + flippedBoundary.Height);
		}

		m_DrawList.push_back(DrawListItem {
			.Texture = command.Texture,
			.TextureSampler = command.TextureSampler,
			.Shader = command.Shader,
			.IndexStart = indicesOffset,
			.IndexCount = command.Indices.size(),
			.ClippingRect = flippedClipRect,
			.BlendMode = command.BlendMode,
			.IsTextCommand = command.IsTextCommand,
		});

		m_Depth += 1.0f / 20'000.0f;
	}

	void Renderer2D::Flush()
	{
		if (m_DrawList.empty())
		{
			return;
		}

		// Bind the vertex array we're using for 2D rendering
		glBindVertexArray(m_VertexArrayId);

		glEnable(GL_BLEND);

		const size_t totalVertices = m_Vertices.size();	//!< Total number of vertices to upload
		const size_t totalIndices = m_Indices.size();	//!< Total number of indices to upload

		size_t vertexOffset	= 0;	//!< Offset in the total vertex buffer
		size_t indexOffset	= 0;	//!< Offset in the total index buffer

		size_t iterations = 0;
		while (indexOffset < totalIndices)
		{
			// Compute how many vertices & indices to upload in this batch
			const size_t verticesInThisBatch = std::min(totalVertices - vertexOffset, m_VertexBufferCapacity);
			const size_t indicesInThisBatch = std::min(totalIndices - indexOffset, m_ElementBufferCapacity);

			// Upload all vertices & indices to the GPU
			glNamedBufferSubData(m_VertexArrayId, 0, verticesInThisBatch * sizeof(Vertex2D), m_Vertices.data() + vertexOffset);
			glNamedBufferSubData(m_ElementBufferId, 0, indicesInThisBatch * sizeof(uint32_t), m_Indices.data() + indexOffset);

			const size_t firstIndexInBatch	= indexOffset;
			const size_t lastIndexInBatch	= indexOffset + indicesInThisBatch;

			for (size_t i = 0; i < m_DrawList.size(); ++i)
			{
				const DrawListItem& item = m_DrawList[i];

				const size_t firstItemIndex	= item.IndexStart;
				const size_t lastItemIndex	= item.IndexStart + item.IndexCount;
				const bool itemFitsInCurrentBatch = (firstItemIndex >= firstIndexInBatch and lastItemIndex <= lastIndexInBatch);

				// Check if this item is within the current batch
				if (not itemFitsInCurrentBatch)
				{
					continue;
				}

				const size_t firstItemIndexInBatch = firstItemIndex - firstIndexInBatch;
				const auto indexOffsetPtr = std::bit_cast<const GLvoid*>(firstItemIndexInBatch * sizeof(uint32_t));

				const bool scissoringRequired = item.ClippingRect.IsClipped();
				if (scissoringRequired)
				{
					const auto& rect = item.ClippingRect.UnwrapBoundary();

					glEnable(GL_SCISSOR_TEST);
					glScissor(rect.Left, rect.Top, rect.Width, rect.Height);
				}

				glBlendFuncSeparate(BlendFactorToGlId(item.BlendMode.SourceColorFactor), BlendFactorToGlId(item.BlendMode.DestinationColorFactor), BlendFactorToGlId(item.BlendMode.SourceAlphaFactor), BlendFactorToGlId(item.BlendMode.DestinationAlphaFactor));
				glBlendEquationSeparate(BlendEquationToGlId(item.BlendMode.ColorEquation), BlendEquationToGlId(item.BlendMode.AlphaEquation));

				glBindSampler(0, item.TextureSampler->GetSamplerId());
				glBindTextureUnit(0, item.Texture->GetTextureId());

				glUseProgram(item.Shader->GetShaderId());
				item.Shader->UploadFloatMatrix4x4("u_ProjectionMatrix", m_ProjectionMatrix.GetData());
				item.Shader->UploadInt1("u_Texture", 0);
				item.Shader->UploadInt1("u_IsText", item.IsTextCommand ? 1 : 0);

				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(item.IndexCount), GL_UNSIGNED_INT, indexOffsetPtr);
				
				if (scissoringRequired) glDisable(GL_SCISSOR_TEST);
			}

			indexOffset += indicesInThisBatch;
			vertexOffset += verticesInThisBatch;
			++iterations;
		}

		Debug(std::format("Renderer2D::Flush() - Flushed {} draw calls in {} iterations", m_DrawList.size(), iterations));

		m_Vertices.clear();
		m_Indices.clear();
		m_DrawList.clear();
	}

	OutlineDefinition Renderer2D::GenerateOutline(
		const std::span<const Math::Float2>& points,
		const std::span<const Math::Float4>& colors,
		const float strokeWeight,
		const float depth
	) {
		std::vector<Math::Float3> meshPositions;
		std::vector<Math::Float4> meshColors;
		std::vector<uint32_t> meshIndices;

		for (size_t i = 0; i < points.size(); ++i)
		{
			const Math::Float2& p0 = points[(i - 1 + points.size()) % points.size()];
			const Math::Float2& p1 = points[i];
			const Math::Float2& p2 = points[(i + 1) % points.size()];

			// Compute the direction vectors
			const Math::Float2 dir1 = (p1 - p0).Normalized();
			const Math::Float2 dir2 = (p2 - p1).Normalized();

			const float cross = dir1.X * dir2.Y - dir1.Y * dir2.X;
			const float sign = (cross >= 0.0f) ? 1.0f : -1.0f;

			// Compute the normal vectors
			const Math::Float2 n1 = dir1.Perpendicular();
			const Math::Float2 n2 = dir2.Perpendicular();

			// Compute the bisector vector
			const Math::Float2 bisector = (n1 + n2).Normalized();
			const float angleCos = n1.Dot(bisector);
			const float miterLength = (strokeWeight * 0.5f) / angleCos;

			switch (StrokeJoin::Miter)
			{
				case StrokeJoin::Miter:
				{
					const Math::Float2 outer = p1 + bisector * miterLength * sign;
					const Math::Float2 inner = p1 - bisector * miterLength * sign;

					meshPositions.emplace_back(outer, depth);
					meshPositions.emplace_back(inner, depth);

					meshColors.emplace_back(colors[i]);
					meshColors.emplace_back(colors[i]);
				} break;

				default: ThrowError("Unknown StrokeJoin in MeshBuilder::GenerateOutlinedMesh()");
			}
		}

		for (size_t i = 0; i < points.size(); ++i)
		{
			const size_t nextIndex = (i + 1) % points.size();
			const uint32_t currentOuterIndex = static_cast<uint32_t>(i * 2);
			const uint32_t currentInnerIndex = static_cast<uint32_t>(i * 2 + 1);
			const uint32_t nextOuterIndex = static_cast<uint32_t>(nextIndex * 2);
			const uint32_t nextInnerIndex = static_cast<uint32_t>(nextIndex * 2 + 1);

			// First triangle
			meshIndices.emplace_back(currentOuterIndex);
			meshIndices.emplace_back(currentInnerIndex);
			meshIndices.emplace_back(nextOuterIndex);

			// Second triangle
			meshIndices.emplace_back(currentInnerIndex);
			meshIndices.emplace_back(nextInnerIndex);
			meshIndices.emplace_back(nextOuterIndex);
		}

		return OutlineDefinition {
			.Positions = std::move(meshPositions),
			.Colors = std::move(meshColors),
			.Indices = std::move(meshIndices)
		};
	}
}