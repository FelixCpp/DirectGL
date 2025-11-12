module;

#include <glad/gl.h>
#include <cstddef>
#include <cmath>
#include <cstdio>
#include <utility>
#include <array>
#include <string_view>

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

uniform mat4 u_ProjectionViewMatrix;

void main()
{
	gl_Position = u_ProjectionViewMatrix * vec4(a_Position, 1.0);
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
}
)";

static constexpr auto FRAMENT_SOURCE = R"(
#version 460 core

layout (location = 0) out vec4 o_FragColor;
layout (location = 0) in vec4 v_Color;
layout (location = 1) in vec2 v_TexCoord;

uniform int u_IsText;
uniform int u_IsSdfText;
uniform sampler2D u_Texture;

void main()
{
	if (u_IsText == 1) {
		if (u_IsSdfText == 1) {
			float distance		= texture(u_Texture, v_TexCoord).r;
			float smoothing		= fwidth(distance);
			float alpha			= smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);
			o_FragColor			= vec4(v_Color.rgb, alpha) * v_Color;
		} else {
			float sampled = texture(u_Texture, v_TexCoord).r;
			o_FragColor = vec4(v_Color.rgb, v_Color.a * sampled);
		}
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
		m_ShaderProgramId(0),
		m_VertexBufferCapacity(1000),
		m_ElementBufferCapacity(1000),
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

		// 2D Texture Coordinates (U, V)
		glEnableVertexArrayAttrib(m_VertexArrayId, 1);
		glVertexArrayAttribFormat(m_VertexArrayId, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, TexCoord));
		glVertexArrayAttribBinding(m_VertexArrayId, 1, 0);

		// 4D Color (R, G, B, A)
		glEnableVertexArrayAttrib(m_VertexArrayId, 2);
		glVertexArrayAttribFormat(m_VertexArrayId, 2, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, Color));
		glVertexArrayAttribBinding(m_VertexArrayId, 2, 0);

		const GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShaderId, 1, &VERTEX_SOURCE, nullptr);
		glCompileShader(vertexShaderId);

		const GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShaderId, 1, &FRAMENT_SOURCE, nullptr);
		glCompileShader(fragmentShaderId);
		m_ShaderProgramId = glCreateProgram();
		glAttachShader(m_ShaderProgramId, vertexShaderId);
		glAttachShader(m_ShaderProgramId, fragmentShaderId);
		glLinkProgram(m_ShaderProgramId);
		glDetachShader(m_ShaderProgramId, vertexShaderId);
		glDetachShader(m_ShaderProgramId, fragmentShaderId);
		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);
		m_ProjectionLocation = glGetProgramResourceLocation(m_ShaderProgramId, GL_UNIFORM, "u_ProjectionViewMatrix");

		constexpr uint8_t whitePixel[4] = { 255, 255, 255, 255 };
		glCreateTextures(GL_TEXTURE_2D, 1, &m_WhiteTextureId);
		glTextureStorage2D(m_WhiteTextureId, 1, GL_RGBA8, 1, 1);
		glTextureSubImage2D(m_WhiteTextureId, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
		glTextureParameteri(m_WhiteTextureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_WhiteTextureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_WhiteTextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_WhiteTextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	Renderer2D::~Renderer2D()
	{
		glDeleteBuffers(1, &m_VertexBufferId);
		glDeleteBuffers(1, &m_ElementBufferId);
		glDeleteVertexArrays(1, &m_VertexArrayId);
		glDeleteProgram(m_ShaderProgramId);
		glDeleteTextures(1, &m_WhiteTextureId);
	}

	void Renderer2D::SetViewport(const Math::FloatBoundary& viewport)
	{
		m_Viewport = viewport;
		m_ProjectionMatrix = Math::Matrix4x4::Orthographic(viewport, -1.0f, 1.0f);
	}

	void Renderer2D::BeginFrame()
	{
		m_Depth = 0.0f;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer2D::EndFrame()
	{
		Flush();
	}

	void Renderer2D::FillRectangle(const Math::FloatBoundary& boundary, const Math::Float4& color, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode)
	{
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
			.ClippingRect = clippingRect,
			.BlendMode = blendMode,
			.TextureId = m_WhiteTextureId,
			.IsText = false,
			.IsSdfText = false,
		});
	}

	void Renderer2D::DrawRectangle(const Math::FloatBoundary& boundary, const Math::Float4& color, const float strokeWeight, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode)
	{
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
			.ClippingRect = clippingRect,
			.BlendMode = blendMode,
			.TextureId = m_WhiteTextureId,
			.IsText = false,
			.IsSdfText = false,
		});
	}

	void Renderer2D::FillEllipse(const Math::Float2& center, const Math::Radius& radius, const Math::Float4& color, const size_t segments, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode)
	{
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
			.ClippingRect = clippingRect,
			.BlendMode = blendMode,
			.TextureId = m_WhiteTextureId,
			.IsText = false,
			.IsSdfText = false,
		});
	}

	void Renderer2D::DrawEllipse(const Math::Float2& center, const Math::Radius& radius, const float strokeWeight, const Math::Float4& color, size_t segments, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode)
	{
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
			.ClippingRect = clippingRect,
			.BlendMode = blendMode,
			.TextureId = m_WhiteTextureId,
			.IsText = false,
			.IsSdfText = false,
		});
	}

	void Renderer2D::FillTriangle(const Math::Float2& p1, const Math::Float2& p2, const Math::Float2& p3, const Math::Float4& color, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode)
	{
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
			.ClippingRect = clippingRect,
			.BlendMode = blendMode,
			.TextureId = m_WhiteTextureId,
			.IsText = false,
			.IsSdfText = false,
		});
	}

	void Renderer2D::DrawTriangle(const Math::Float2& p1, const Math::Float2& p2, const Math::Float2& p3, const float strokeWeight, const Math::Float4& color, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode)
	{
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
			.ClippingRect = clippingRect,
			.BlendMode = blendMode,
			.TextureId = m_WhiteTextureId,
			.IsText = false,
			.IsSdfText = false,
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

	void Renderer2D::FillLine(const Math::Float2& p1, const Math::Float2& p2, const float strokeWeight, const Math::Float4& color, StrokeCap strokeCap, size_t roundedStrokeCapSegments, const ClipRect& clippingRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode)
	{
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
			.ClippingRect = clippingRect,
			.BlendMode = blendMode,
			.TextureId = m_WhiteTextureId,
			.IsText = false,
			.IsSdfText = false,
		});
	}

	void Renderer2D::DrawText(const std::string_view text, const Font& font, const float fontSize, const Math::Float2& position, const Math::Float4& color, const ClipRect& clipRect, const Math::Matrix4x4& modelMatrix, const BlendMode& blendMode)
	{
		//if (false) {
		//	// Draw the entire texture for debugging purposes
		//	const Math::Float3 positions[] = {
		//		Math::Float3{ 100.0f, 100.0f, m_Depth },
		//		Math::Float3{ 300.0f, 100.0f, m_Depth },
		//		Math::Float3{ 300.0f, 300.0f, m_Depth },
		//		Math::Float3{ 100.0f, 300.0f, m_Depth },
		//	};
		//
		//	const Glyph* a = font.GetGlyph('j');
		//
		//	const Math::Float2 textureCoordinates[] = {
		//		Math::Float2 { a->TextureCoordinates.TopLeft() },
		//		Math::Float2 { a->TextureCoordinates.TopRight()},
		//		Math::Float2 { a->TextureCoordinates.BottomRight()},
		//		Math::Float2 { a->TextureCoordinates.BottomLeft() },
		//	};
		//
		//	const std::vector vertices = {
		//		Vertex2D{.Position = positions[0], .TexCoord = textureCoordinates[0], .Color = color },
		//		Vertex2D{.Position = positions[1], .TexCoord = textureCoordinates[1], .Color = color },
		//		Vertex2D{.Position = positions[2], .TexCoord = textureCoordinates[2], .Color = color },
		//		Vertex2D{.Position = positions[3], .TexCoord = textureCoordinates[3], .Color = color },
		//	};
		//
		//	const std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };
		//
		//	AddDrawCommand(DrawCommand2D{
		//		.Vertices = std::span(vertices),
		//		.Indices = std::span(indices),
		//		.ClippingRect = clipRect,
		//		.BlendMode = blendMode,
		//		.TextureId = font.GetTextureId(),
		//		.IsText = true,
		//	});
		//
		//	return;
		//}

		// Compute the font scale we need to apply for the glyphs to be rendered at the desired size
		const float fontScale = font.GetPixelScale(fontSize);

		// Define a position that we will update as we render each character
		Math::Float2 pos = position;

		std::vector<Vertex2D> vertices;
		for (const char character : text)
		{
			//if (character == '\n')
			//{
			//	pos.X = position.X;
			//	pos.Y += font.GetLineHeight();
			//	continue;
			//}

			const Glyph* glyph = font.GetGlyph(character);
			//if (glyph == nullptr)
			//{
			//	glyph = font.GetGlyph(' ');
			//	if (glyph == nullptr) continue;
			//}
			//
			//if (character == ' ')
			//{
			//	pos.X += static_cast<float>(glyph->Advance.X) * fontScale;
			//	continue;
			//}

			// Compute the position and size of the glyph quad
			const float xpos	= pos.X + glyph->Bearing.Left * fontScale;
			const float ypos	= pos.Y - glyph->Bearing.Top * fontScale;
			const float width	= glyph->TextureSize.X * fontScale;
			const float height	= glyph->TextureSize.Y * fontScale;

			// Define the vertices for the glyph quad
			const Math::Float3 positions[] = {
				modelMatrix.TransformPoint(Math::Float3{ xpos,         ypos + height, m_Depth }),
				modelMatrix.TransformPoint(Math::Float3{ xpos + width, ypos + height, m_Depth }),
				modelMatrix.TransformPoint(Math::Float3{ xpos + width, ypos         , m_Depth }),
				modelMatrix.TransformPoint(Math::Float3{ xpos,         ypos         , m_Depth }),
			};

			// Define the texture coordinates for the glyph quad
			const auto [uvLeft, uvTop, uvWidth, uvHeight] = glyph->TextureCoordinates;
			const Math::Float2 textureCoordinates[] = {
				Math::Float2{ uvLeft,           uvTop + uvHeight },
				Math::Float2{ uvLeft + uvWidth, uvTop + uvHeight },
				Math::Float2{ uvLeft + uvWidth, uvTop            },
				Math::Float2{ uvLeft,           uvTop            },
			};

			// Convert the glyph quad into vertices
			Vertex2D glyphVertices[] = {
				Vertex2D{ .Position = positions[0], .TexCoord = textureCoordinates[0], .Color = color },
				Vertex2D{ .Position = positions[1], .TexCoord = textureCoordinates[1], .Color = color },
				Vertex2D{ .Position = positions[2], .TexCoord = textureCoordinates[2], .Color = color },
				Vertex2D{ .Position = positions[3], .TexCoord = textureCoordinates[3], .Color = color },
			};

			// Append the glyph vertices to the vertex list
			vertices.insert_range(vertices.end(), glyphVertices);

			pos.X += static_cast<float>(glyph->Advance.X) * fontScale;
		}

		// Generate indices in 0, 1, 2, 2, 3, 0 pattern for quads
		const size_t quadCount = vertices.size() / 4;
		std::vector<uint32_t> indices;
		indices.reserve(quadCount * 6);
		for (size_t i = 0; i < quadCount; ++i)
		{
			const uint32_t vertexOffset = static_cast<uint32_t>(i * 4);
			indices.push_back(vertexOffset + 0);
			indices.push_back(vertexOffset + 1);
			indices.push_back(vertexOffset + 2);
			indices.push_back(vertexOffset + 2);
			indices.push_back(vertexOffset + 3);
			indices.push_back(vertexOffset + 0);
		}

		AddDrawCommand(DrawCommand2D {
			.Vertices = std::span(vertices),
			.Indices = std::span(indices),
			.ClippingRect = clipRect,
			.BlendMode = blendMode,
			.TextureId = font.GetTextureId(),
			.IsText = true,
			.IsSdfText = true,
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

		// Append the vertices
		m_Vertices.insert(m_Vertices.end(), command.Vertices.begin(), command.Vertices.end());

		// Append the indices
		for (const uint32_t index : command.Indices) m_Indices.push_back(index + verticesOffset);

		// Flip Y for clipping rect
		std::optional<Math::IntBoundary> flippedClippingRect = command.ClippingRect;
		if (flippedClippingRect.has_value())
		{
			flippedClippingRect->Top = static_cast<int>(m_Viewport.Height) - (flippedClippingRect->Top + flippedClippingRect->Height);
		}

		m_DrawList.push_back(DrawListItem {
			.TextureId = command.TextureId,
			.IndexCount = command.Indices.size(),
			.ClippingRect = flippedClippingRect,
			.BlendMode = command.BlendMode,
			.IsText = command.IsText,
			.IsSdfText = command.IsSdfText,
		});

		m_Depth += 1.0f / 20'000.0f;
	}

	void Renderer2D::Flush()
	{
		if (m_DrawList.empty())
		{
			return;
		}

		// Enable blending (additive)

		// Upload all vertices & indices to the GPU
		glNamedBufferSubData(m_VertexArrayId, 0, m_Vertices.size() * sizeof(Vertex2D), m_Vertices.data());
		glNamedBufferSubData(m_ElementBufferId, 0, m_Indices.size() * sizeof(uint32_t), m_Indices.data());

		// Bind the vertex array we're using for 2D rendering
		glBindVertexArray(m_VertexArrayId);
		glUseProgram(m_ShaderProgramId);
		glProgramUniformMatrix4fv(m_ShaderProgramId, m_ProjectionLocation, 1, GL_FALSE, m_ProjectionMatrix.GetData());

		glEnable(GL_BLEND);
		
		size_t drawOffset = 0;
		for (size_t i = 0; i < m_DrawList.size(); ++i)
		{
			DrawListItem& item = m_DrawList[i];
			const bool scissoringRequired = item.ClippingRect.has_value();

			if (scissoringRequired)
			{
				glEnable(GL_SCISSOR_TEST);
				glScissor(item.ClippingRect->Left, item.ClippingRect->Top, item.ClippingRect->Width, item.ClippingRect->Height);
			}

			glBlendFuncSeparate(
				BlendFactorToGlId(item.BlendMode.SourceColorFactor),
				BlendFactorToGlId(item.BlendMode.DestinationColorFactor),
				BlendFactorToGlId(item.BlendMode.SourceAlphaFactor),
				BlendFactorToGlId(item.BlendMode.DestinationAlphaFactor)
			);

			glBlendEquationSeparate(
				BlendEquationToGlId(item.BlendMode.ColorEquation),
				BlendEquationToGlId(item.BlendMode.AlphaEquation)
			);

			glBindTextureUnit(0, item.TextureId);
			glProgramUniform1i(m_ShaderProgramId, glGetProgramResourceLocation(m_ShaderProgramId, GL_UNIFORM, "u_IsSdfText"), item.IsSdfText ? 1 : 0);
			glProgramUniform1i(m_ShaderProgramId, glGetProgramResourceLocation(m_ShaderProgramId, GL_UNIFORM, "u_IsText"), item.IsText ? 1 : 0);

			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(item.IndexCount), GL_UNSIGNED_INT, reinterpret_cast<void*>(drawOffset * sizeof(uint32_t)));
			if (scissoringRequired) glDisable(GL_SCISSOR_TEST);
			drawOffset += item.IndexCount;
		}

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