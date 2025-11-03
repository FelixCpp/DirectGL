module;

#include <glad/gl.h>
#include <span>
#include <memory>
#include <array>

module DirectGL;

import :AdvancedGraphicsLayer;

namespace DGL
{
	struct Vertex
	{
		Math::Float3 Position;
		Math::Float3 TexCoord;
		Renderer::Color Tint;
	};
}

namespace DGL
{
	class RectangleRenderer
	{
	private:

		static constexpr auto VERTEX_SOURCE = R"(
			#version 460 core

			layout (location = 0) in vec3 a_Position;
			layout (location = 1) in vec2 a_TexCoord;
			layout (location = 2) in vec4 a_Color;

			layout (location = 0) out vec2 v_TexCoord;
			layout (location = 1) out vec4 v_Color;

			uniform mat4 u_ProjectionViewMatrix;

			void main() {
				gl_Position = u_ProjectionViewMatrix * vec4(a_Position, 1.0);
				v_TexCoord = a_TexCoord;
				v_Color = a_Color;
			}
		)";

		static constexpr auto FRAGMENT_SOURCE = R"(
			#version 460 core

			layout (location = 0) in vec2 v_TexCoord;
			layout (location = 1) in vec4 v_Color;

			layout (location = 0) out vec4 o_FragColor;

			void main() {
				o_FragColor = v_Color;
			}
		)";

	public:

		explicit RectangleRenderer(const size_t maxBatchSize):
			m_VertexArray(0),
			m_PositionsBuffer(0),
			m_TexCoordsBuffer(0),
			m_ColorsBuffer(0),
			m_ElementBuffer(0),
			m_BatchedRectangles(0),
			m_MaxBatchSize(maxBatchSize),
			m_Positions(std::make_unique<GLfloat[]>(maxBatchSize * 4 * 3)),
			m_TexCoords(std::make_unique<GLfloat[]>(maxBatchSize * 4 * 2)),
			m_Colors(std::make_unique<GLfloat[]>(maxBatchSize * 4 * 4))
		{
			glCreateBuffers(1, &m_PositionsBuffer);
			glNamedBufferStorage(m_PositionsBuffer, maxBatchSize * 4 * 3 * sizeof(GLfloat), nullptr, GL_DYNAMIC_STORAGE_BIT);

			glCreateBuffers(1, &m_TexCoordsBuffer);
			glNamedBufferStorage(m_TexCoordsBuffer, maxBatchSize * 4 * 2 * sizeof(GLfloat), nullptr, GL_DYNAMIC_STORAGE_BIT);

			glCreateBuffers(1, &m_ColorsBuffer);
			glNamedBufferStorage(m_ColorsBuffer, maxBatchSize * 4 * 4 * sizeof(GLfloat), nullptr, GL_DYNAMIC_STORAGE_BIT);

			const auto indices = std::make_unique<uint32_t[]>(maxBatchSize * 6);
			for (size_t i = 0; i < maxBatchSize; ++i)
			{
				const uint32_t baseIndex = static_cast<uint32_t>(i * 6);
				const uint32_t baseVertex = static_cast<uint32_t>(i * 4);
				indices[baseIndex + 0] = baseVertex + 0;
				indices[baseIndex + 1] = baseVertex + 1;
				indices[baseIndex + 2] = baseVertex + 2;
				indices[baseIndex + 3] = baseVertex + 2;
				indices[baseIndex + 4] = baseVertex + 3;
				indices[baseIndex + 5] = baseVertex + 0;
			}

			glCreateBuffers(1, &m_ElementBuffer);
			glNamedBufferStorage(m_ElementBuffer, maxBatchSize * 6 * sizeof(uint32_t), indices.get(), GL_DYNAMIC_STORAGE_BIT);

			glCreateVertexArrays(1, &m_VertexArray);
			glVertexArrayElementBuffer(m_VertexArray, m_ElementBuffer);

			glVertexArrayVertexBuffer(m_VertexArray, 0, m_PositionsBuffer, 0, 3 * sizeof(GLfloat));
			glEnableVertexArrayAttrib(m_VertexArray, 0);
			glVertexArrayAttribFormat(m_VertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
			glVertexArrayAttribBinding(m_VertexArray, 0, 0);

			glVertexArrayVertexBuffer(m_VertexArray, 1, m_TexCoordsBuffer, 0, 2 * sizeof(GLfloat));
			glEnableVertexArrayAttrib(m_VertexArray, 1);
			glVertexArrayAttribFormat(m_VertexArray, 1, 2, GL_FLOAT, GL_FALSE, 0);
			glVertexArrayAttribBinding(m_VertexArray, 1, 1);

			glVertexArrayVertexBuffer(m_VertexArray, 2, m_ColorsBuffer, 0, 4 * sizeof(GLfloat));
			glEnableVertexArrayAttrib(m_VertexArray, 2);
			glVertexArrayAttribFormat(m_VertexArray, 2, 4, GL_FLOAT, GL_FALSE, 0);
			glVertexArrayAttribBinding(m_VertexArray, 2, 2);

			const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &VERTEX_SOURCE, nullptr);
			glCompileShader(vertexShader);

			const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &FRAGMENT_SOURCE, nullptr);
			glCompileShader(fragmentShader);

			m_ShaderProgramId = glCreateProgram();
			glAttachShader(m_ShaderProgramId, vertexShader);
			glAttachShader(m_ShaderProgramId, fragmentShader);
			glLinkProgram(m_ShaderProgramId);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			m_ProjectionViewMatrixLocation = glGetUniformLocation(m_ShaderProgramId, "u_ProjectionViewMatrix");
		}

		~RectangleRenderer()
		{
			glDeleteProgram(m_ShaderProgramId);
			glDeleteBuffers(1, &m_ElementBuffer);
			glDeleteBuffers(1, &m_ColorsBuffer);
			glDeleteBuffers(1, &m_TexCoordsBuffer);
			glDeleteBuffers(1, &m_PositionsBuffer);
			glDeleteVertexArrays(1, &m_VertexArray);
		}

		void BeginDraw(const Math::Matrix4x4& projectionViewMatrix)
		{
			glUseProgram(m_ShaderProgramId);
			glProgramUniformMatrix4fv(m_ShaderProgramId, m_ProjectionViewMatrixLocation, 1, GL_FALSE, projectionViewMatrix.GetData());
		}

		void EndDraw()
		{
			if (m_BatchedRectangles > 0)
			{
				Flush();
			}
		}

		void Submit(
			const std::span<const GLfloat, 3 * 4>& positions,
			const std::span<const GLfloat, 2 * 4>& texCoords,
			const std::span<const GLfloat, 4 * 4>& colors
		) {
			const size_t positionsOffset = m_BatchedRectangles * 4 * 3;
			const size_t texCoordsOffset = m_BatchedRectangles * 4 * 2;
			const size_t colorsOffset = m_BatchedRectangles * 4 * 4;

			// Check if we need to flush the batch
			if (m_BatchedRectangles >= m_MaxBatchSize)
			{
				Flush();
			}

			// Copy the data into their corresponding batch location
			std::ranges::copy(positions, m_Positions.get() + positionsOffset);
			std::ranges::copy(texCoords, m_TexCoords.get() + texCoordsOffset);
			std::ranges::copy(colors, m_Colors.get() + colorsOffset);

			++m_BatchedRectangles;
		}

	private:

		void Flush()
		{
			// Upload the data to the GPU
			glNamedBufferSubData(m_PositionsBuffer, 0, m_BatchedRectangles * 4 * 3 * sizeof(GLfloat), m_Positions.get());
			glNamedBufferSubData(m_TexCoordsBuffer, 0, m_BatchedRectangles * 4 * 2 * sizeof(GLfloat), m_TexCoords.get());
			glNamedBufferSubData(m_ColorsBuffer, 0, m_BatchedRectangles * 4 * 4 * sizeof(GLfloat), m_Colors.get());

			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_BatchedRectangles * 6), GL_UNSIGNED_INT, nullptr);

			m_BatchedRectangles = 0;
		}

	private:

		GLuint m_VertexArray;

		GLuint m_PositionsBuffer;
		GLuint m_TexCoordsBuffer;
		GLuint m_ColorsBuffer;

		GLuint m_ElementBuffer;

		GLuint m_ShaderProgramId;
		GLint m_ProjectionViewMatrixLocation;

		size_t m_BatchedRectangles;
		size_t m_MaxBatchSize;

		std::unique_ptr<GLfloat[]> m_Positions;
		std::unique_ptr<GLfloat[]> m_TexCoords;
		std::unique_ptr<GLfloat[]> m_Colors;

		

	};

	static RectangleRenderer* s_RectangleRenderer;
}

namespace DGL
{
	AdvancedGraphicsLayer::AdvancedGraphicsLayer(const Math::Uint2& viewportSize):
		m_Viewport(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(viewportSize.X), static_cast<float>(viewportSize.Y))),
		m_RenderTarget(Renderer::MainRenderTarget::Create(static_cast<Math::UintBoundary>(m_Viewport)))
	{
		s_RectangleRenderer = new RectangleRenderer(1000); // Arbitrary batch size
	}

	void AdvancedGraphicsLayer::Resize(const Math::Uint2& viewportSize)
	{
		m_Viewport = Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(viewportSize.X), static_cast<float>(viewportSize.Y));
		m_RenderTarget->SetViewport(static_cast<Math::UintBoundary>(m_Viewport));
	}

	void AdvancedGraphicsLayer::BeginDraw()
	{
		s_RectangleRenderer->BeginDraw(Math::Matrix4x4::Orthographic(m_Viewport, -1.0f, 1.0f));
		m_RenderTarget->Activate();
	}

	void AdvancedGraphicsLayer::EndDraw()
	{
		s_RectangleRenderer->EndDraw();
	}

	void AdvancedGraphicsLayer::Suspend()
	{
	}

	void AdvancedGraphicsLayer::Resume()
	{
		m_RenderTarget->Activate();
	}

	const Math::FloatBoundary& AdvancedGraphicsLayer::GetViewport() const
	{
		return m_Viewport;
	}

	void AdvancedGraphicsLayer::PushState()
	{
		m_RenderStateStack.PushState();
	}

	void AdvancedGraphicsLayer::PopState()
	{
		m_RenderStateStack.PopState();
	}

	RenderState& AdvancedGraphicsLayer::PeekState()
	{
		return m_RenderStateStack.PeekState();
	}

	void AdvancedGraphicsLayer::PushTransform()
	{
		PeekState().TransformationStack.PushTransform();
	}

	void AdvancedGraphicsLayer::PopTransform()
	{
		PeekState().TransformationStack.PopTransform();
	}

	Math::Matrix4x4& AdvancedGraphicsLayer::PeekTransform()
	{
		return PeekState().TransformationStack.PeekTransform();
	}

	void AdvancedGraphicsLayer::ResetTransform()
	{
		PeekTransform() = Math::Matrix4x4::Identity;
	}

	void AdvancedGraphicsLayer::Translate(const float x, const float y)
	{
		PeekTransform() *= Math::Matrix4x4::Translation(x, y, 0.0f);
	}

	void AdvancedGraphicsLayer::Scale(const float x, const float y)
	{
		PeekTransform() *= Math::Matrix4x4::Scaling(x, y, 1.0f);
	}

	void AdvancedGraphicsLayer::Rotate(const Math::Angle angle)
	{
		PeekTransform() *= Math::Matrix4x4::Rotation(angle);
	}

	void AdvancedGraphicsLayer::Skew(const Math::Angle angleX, const Math::Angle angleY)
	{
		PeekTransform() *= Math::Matrix4x4::Skew(angleX, angleY);
	}

	void AdvancedGraphicsLayer::Fill(const Renderer::Color color)
	{
		RenderState& state = PeekState();
		state.FillColor = color;
		state.IsFillEnabled = true;
	}

	void AdvancedGraphicsLayer::Stroke(const Renderer::Color color)
	{
		RenderState& state = PeekState();
		state.StrokeColor = color;
		state.IsStrokeEnabled = true;
	}

	void AdvancedGraphicsLayer::StrokeWeight(const float strokeWeight)
	{
		PeekState().StrokeWeight = strokeWeight;
	}

	void AdvancedGraphicsLayer::NoFill()
	{
		PeekState().IsFillEnabled = false;
	}

	void AdvancedGraphicsLayer::NoStroke()
	{
		PeekState().IsStrokeEnabled = false;
	}

	void AdvancedGraphicsLayer::SetBlendMode(const Blending::BlendMode& blendMode)
	{
		PeekState().BlendMode = blendMode;
	}

	void AdvancedGraphicsLayer::SetRectMode(const RectMode& rectMode)
	{
		PeekState().RectMode = rectMode;
	}

	void AdvancedGraphicsLayer::SetImageMode(const RectMode& imageMode)
	{
		PeekState().ImageMode = imageMode;
	}

	void AdvancedGraphicsLayer::SetEllipseMode(const EllipseMode& ellipseMode)
	{
		PeekState().EllipseMode = ellipseMode;
	}

	void AdvancedGraphicsLayer::SetEllipseSegmentCountMode(const SegmentCountMode& segmentCountMode)
	{
		PeekState().EllipseSegmentCountMode = segmentCountMode;
	}

	void AdvancedGraphicsLayer::SetLineStartCap(const ShapeRenderer::LineCapStyle startCap)
	{
		PeekState().LineStartCap = startCap;
	}

	void AdvancedGraphicsLayer::SetLineEndCap(const ShapeRenderer::LineCapStyle endCap)
	{
		PeekState().LineEndCap = endCap;
	}

	void AdvancedGraphicsLayer::SetLineJoinStyle(const ShapeRenderer::LineJoinStyle joinStyle)
	{
		PeekState().LineJoinStyle = joinStyle;
	}

	void AdvancedGraphicsLayer::SetLineSegmentCountMode(const SegmentCountMode& segmentCountMode)
	{
		PeekState().LineSegmentCountMode = segmentCountMode;
	}

	void AdvancedGraphicsLayer::SetImageTint(const Renderer::Color tint)
	{
		PeekState().ImageTint = tint;
	}

	void AdvancedGraphicsLayer::SetImageAlpha(const uint8_t alpha)
	{
		PeekState().ImageAlpha = alpha;
	}

	void AdvancedGraphicsLayer::SetImageOpacity(const float opacity)
	{
		SetImageAlpha(opacity * 255.0f); // TODO(Felix): Clamp value between 0 and 255
	}

	void AdvancedGraphicsLayer::SetImageFilterMode(const Texture::TextureFilterMode filterMode)
	{
		PeekState().ImageFilterMode = filterMode;
	}

	void AdvancedGraphicsLayer::SetImageWrapMode(const Texture::TextureWrapMode wrapMode)
	{
		PeekState().ImageWrapMode = wrapMode;
	}

	void AdvancedGraphicsLayer::Background(const Renderer::Color color)
	{
		glClearColor(
			static_cast<float>(color.R) / 255.0f,
			static_cast<float>(color.G) / 255.0f,
			static_cast<float>(color.B) / 255.0f,
			static_cast<float>(color.A) / 255.0f
		);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void AdvancedGraphicsLayer::Rect(const float x1, const float y1, const float x2, const float y2)
	{
		auto& state = PeekState();
		const auto& transform = state.TransformationStack.PeekTransform();
		const auto boundary = state.RectMode(x1, y1, x2, y2);
		const Math::Float2 topLeft = transform.TransformPoint(boundary.TopLeft());
		const Math::Float2 topRight = transform.TransformPoint(boundary.TopRight());
		const Math::Float2 bottomRight = transform.TransformPoint(boundary.BottomRight());
		const Math::Float2 bottomLeft = transform.TransformPoint(boundary.BottomLeft());
		constexpr float depth = 0.0f; // TODO(Felix): Use actual depth from depth provider

		const float r = state.FillColor.R / 255.0f;
		const float g = state.FillColor.G / 255.0f;
		const float b = state.FillColor.B / 255.0f;
		const float a = state.FillColor.A / 255.0f;

		const std::array positions = {
			topLeft.X,     topLeft.Y,     depth,
			topRight.X,    topRight.Y,    depth,
			bottomRight.X, bottomRight.Y, depth,
			bottomLeft.X,  bottomLeft.Y,  depth
		};

		constexpr std::array texCoords = {
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
		};

		const std::array colors = {
			r, g, b, a,
			r, g, b, a,
			r, g, b, a,
			r, g, b, a
		};

		s_RectangleRenderer->Submit(std::span(positions), std::span(texCoords), std::span(colors));
	}

	void AdvancedGraphicsLayer::Ellipse(float x1, float y1, float x2, float y2)
	{
	}

	void AdvancedGraphicsLayer::Point(float x, float y)
	{
	}

	void AdvancedGraphicsLayer::Line(float x1, float y1, float x2, float y2)
	{
	}

	void AdvancedGraphicsLayer::Triangle(float x1, float y1, float x2, float y2, float x3, float y3)
	{
	}

	void AdvancedGraphicsLayer::Image(const Texture::Texture& texture, float x1, float y1, float x2, float y2)
	{
	}
}
