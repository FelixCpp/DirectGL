module;

#include <glad/gl.h>
#include <functional>
#include <format>
#include <span>
#include <optional>
#include <chrono>
#include <array>

#include <tesselator.h>

module DirectGL;

import :HybridRenderer;
import DirectGL.ControlFlow;

namespace
{
	inline static constexpr auto GENERIC_VERTEX_SOURCE = R"(
		#version 460 core

		layout (location = 0) in vec3 a_Position;
		layout (location = 1) in vec4 a_Color;

		layout (location = 0) out vec4 v_Color;

		uniform mat4 u_ProjectionMatrix;

		void main()
		{
			gl_Position = u_ProjectionMatrix * vec4(a_Position, 1.0);
			v_Color = a_Color;
		}
	)";

	inline static constexpr auto GENERIC_FRAGMENT_SOURCE = R"(
		#version 460 core

		layout (location = 0) out vec4 o_FragColor;
		layout (location = 0) in vec4 v_Color;

		void main()
		{
			o_FragColor = v_Color;
		}
	)";

	inline static constexpr auto SDF_CIRCLE_VERTEX_SOURCE = R"(
		#version 460 core
		layout (location = 0) in vec3 a_WorldPosition;
		layout (location = 1) in vec2 a_LocalPosition;
		layout (location = 2) in vec4 a_FillColor;
		layout (location = 3) in vec4 a_StrokeColor;
		layout (location = 4) in float a_StrokeWeight;
		layout (location = 5) in vec2 a_CircleSize;

		layout (location = 0) out vec2 v_LocalPosition;
		layout (location = 1) out vec4 v_FillColor;
		layout (location = 2) out vec4 v_StrokeColor;
		layout (location = 3) out float v_StrokeWeight;
		layout (location = 4) out vec2 v_CircleSize;

		uniform mat4 u_ProjectionMatrix;

		void main()
		{
			gl_Position = u_ProjectionMatrix * vec4(a_WorldPosition, 1.0);
			v_LocalPosition = a_LocalPosition;
			v_FillColor = a_FillColor;
			v_StrokeColor = a_StrokeColor;
			v_StrokeWeight = a_StrokeWeight;
			v_CircleSize = a_CircleSize;
		}
	)";

	inline static constexpr auto SDF_CIRCLE_FRAGMENT_SOURCE = R"(
		#version 460 core

		layout (location = 0) out vec4 o_FragColor;
	
		layout (location = 0) in vec2 v_LocalPosition;
		layout (location = 1) in vec4 v_FillColor;
		layout (location = 2) in vec4 v_StrokeColor;
		layout (location = 3) in float v_StrokeWeight;
		layout (location = 4) in vec2 v_CircleSize;

		float sdfEllipse(vec2 position, float halfWidth, float halfHeight, int subdivisionLevel) {
		    position = abs(position);
		    vec2 size = vec2(halfWidth, halfHeight);
		    float startAngle = 0.0;
		    float endAngle = 3.141592 * 0.5;
		    vec2 point1 = vec2(halfWidth, 0.0);
		    vec2 point2 = vec2(0.0, halfHeight);
		    vec2 normal1 = vec2(1.0, 0.0);
		    vec2 normal2 = vec2(0.0, 1.0);
		    for (int i = 0; i < subdivisionLevel; i++) { // perform binary search to find closest segment
		        float angle = (startAngle + endAngle) * 0.5;
		        vec2 point = size * vec2(cos(angle), sin(angle)); 
		        vec2 normal = point / (size * size);
		        if (dot(position - point, vec2(-normal.y, normal.x)) < 0.0) {
		            endAngle = angle;
		            point2 = point;
		            normal2 = normal;
		        } else {
		            startAngle = angle;
		            point1 = point;
		            normal1 = normal;
		        }
		    }
		    float t = dot(point2 - point1, normal2) / dot(vec2(-normal1.y, normal1.x), normal2);
		    vec2 point3 = point1 + vec2(-normal1.y, normal1.x) * t;
		    float a = length(point2 - point3);
		    float b = length(point3 - point1);
		    float c = length(point1 - point2);
		    vec2 incenter = (point1 * a + point2 * b + point3 * c) / (a + b + c);
		    vec2 midpoint1 = (point1 + incenter) * 0.5;
		    vec2 bisector1 = point1 - incenter;
		    bisector1 = vec2(-bisector1.y, bisector1.x);
		    float t1 = dot(point1 - midpoint1, vec2(-normal1.y, normal1.x)) / dot(bisector1, vec2(-normal1.y, normal1.x));
		    vec2 center1 = midpoint1 + bisector1 * t1;
		    vec2 normal3 = incenter - center1;
		    if (dot(position - center1, vec2(-normal3.y, normal3.x)) < 0.0) {
		        float radius1 = length(point1 - center1);
		        return length(position - center1) - radius1;  
		    }
		    vec2 midpoint2 = (point2 + incenter) * 0.5;
		    vec2 bisector2 = point2 - incenter;
		    bisector2 = vec2(-bisector2.y, bisector2.x);
		    float t2 = dot(point2 - midpoint2, vec2(-normal2.y, normal2.x)) / dot(bisector2, vec2(-normal2.y, normal2.x));
		    vec2 center2 = midpoint2 + bisector2 * t2;
		    float radius2 = length(point2 - center2);  
		    return length(position - center2) - radius2;
		}

		//float sdbEllipsoidV2( in vec3 p, in vec3 r )
		//{
		//    float k1 = length(p/r);
		//    float k2 = length(p/(r*r));
		//    return k1*(k1-1.0)/k2;
		//}

		uniform int u_IsFillEnabled;
		uniform int u_IsStrokeEnabled;

		void main()
		{
			const int subdivisions = 1;
			const vec2 position = v_LocalPosition * v_CircleSize;
			const float distance = sdfEllipse(position, v_CircleSize.x, v_CircleSize.y, subdivisions);
			//const float distance = sdbEllipsoidV2(vec3(position, 0.0), vec3(v_CircleSize, 1.0));
			const float fadeWidth = fwidth(distance);
			const float fillFadeStart = -fadeWidth;
			const float fillFadeEnd = 0.0;
			const float fillAlpha = 1.0 - smoothstep(fillFadeStart, fillFadeEnd, distance);
			const vec4 fillColor = vec4(v_FillColor.rgb, v_FillColor.a * fillAlpha);

			if ((u_IsFillEnabled != 0) && (u_IsStrokeEnabled == 0))
			{
				o_FragColor = fillColor;
				return;
			}

			const float strokeWeight = v_StrokeWeight * 2.0f;
			const float innerStrokeFadeStart = -strokeWeight;
			const float innerStrokeFadeEnd = -(strokeWeight - fadeWidth);
			const float strokeAlphaInner = smoothstep(innerStrokeFadeStart, innerStrokeFadeEnd, distance);
			
			const float outerStrokeFadeStart = -fadeWidth;
			const float outerStrokeFadeEnd = 0.0;
			const float strokeAlphaOuter = 1.0 - smoothstep(outerStrokeFadeStart, outerStrokeFadeEnd, distance);
			const float strokeAlpha = strokeAlphaInner * strokeAlphaOuter;
			const vec4 strokeColor = vec4(v_StrokeColor.rgb, v_StrokeColor.a * strokeAlpha);

			if ((u_IsFillEnabled == 0) && (u_IsStrokeEnabled != 0))
			{
				o_FragColor = strokeColor;
				return;
			}

			o_FragColor = mix(fillColor, strokeColor, strokeAlpha);
 		}
	)";

	inline static constexpr auto SDF_ROUNDED_RECT_VERTEX_SOURCE = R"(
		#version 460 core

		layout (location = 0) in vec3 a_WorldPosition;
		layout (location = 1) in vec2 a_LocalPosition;
		layout (location = 2) in vec4 a_FillColor;
		layout (location = 3) in vec4 a_StrokeColor;
		layout (location = 4) in float a_StrokeWeight;
		layout (location = 5) in vec2 a_RectSize;
		layout (location = 6) in float a_BorderRadius;

		layout (location = 0) out vec2 v_LocalPosition;
		layout (location = 1) out vec4 v_FillColor;
		layout (location = 2) out vec4 v_StrokeColor;
		layout (location = 3) out float v_StrokeWeight;
		layout (location = 4) out vec2 v_RectSize;
		layout (location = 5) out float v_BorderRadius;

		uniform mat4 u_ProjectionMatrix;

		void main()
		{
			gl_Position = u_ProjectionMatrix * vec4(a_WorldPosition, 1.0);
			v_LocalPosition = a_LocalPosition;
			v_FillColor = a_FillColor;
			v_StrokeColor = a_StrokeColor;
			v_StrokeWeight = a_StrokeWeight;
			v_RectSize = a_RectSize;
			v_BorderRadius = a_BorderRadius;
		}
	)";

	inline static constexpr auto SDF_ROUNDED_RECT_FRAGMENT_SOURCE = R"(
		#version 460 core

		layout (location = 0) out vec4 o_FragColor;
	
		layout (location = 0) in vec2 v_LocalPosition;
		layout (location = 1) in vec4 v_FillColor;
		layout (location = 2) in vec4 v_StrokeColor;
		layout (location = 3) in float v_StrokeWeight;
		layout (location = 4) in vec2 v_RectSize;
		layout (location = 5) in float v_BorderRadius;

		float RectSDF(vec2 p, vec2 b, float r)
		{
		    vec2 d = abs(p) - b + vec2(r);
		    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - r;
		}
	
		uniform int u_IsFillEnabled;
		uniform int u_IsStrokeEnabled;

		void main()
		{
			float cappedRadius = min(v_BorderRadius, min(v_RectSize.x, v_RectSize.y) * 0.5);
			const float distance = RectSDF(v_LocalPosition * v_RectSize, v_RectSize, cappedRadius);
			const float fadeWidth = fwidth(distance);

			const float fillFadeStart = -fadeWidth;
			const float fillFadeEnd = 0.0;
			const float fillAlpha = 1.0 - smoothstep(fillFadeStart, fillFadeEnd, distance);
			vec4 fillColor = vec4(v_FillColor.rgb, v_FillColor.a * fillAlpha);
		
			if ((u_IsFillEnabled != 0) && (u_IsStrokeEnabled == 0))
			{
				o_FragColor = fillColor;
				return;
			}

			const float strokeWeight = v_StrokeWeight * 2.0;
			const float innerStrokeFadeStart = -strokeWeight;
			const float innerStrokeFadeEnd = -(strokeWeight - fadeWidth);
			const float strokeAlphaInner = smoothstep(innerStrokeFadeStart, innerStrokeFadeEnd, distance);
			
			const float outerStrokeFadeStart = -fadeWidth;
			const float outerStrokeFadeEnd = 0.0;
			const float strokeAlphaOuter = 1.0 - smoothstep(outerStrokeFadeStart, outerStrokeFadeEnd, distance);
			const float strokeAlpha = strokeAlphaInner * strokeAlphaOuter;
			const vec4 strokeColor = vec4(v_StrokeColor.rgb, v_StrokeColor.a * strokeAlpha);
			
			if ((u_IsFillEnabled == 0) && (u_IsStrokeEnabled != 0))
			{
				o_FragColor = strokeColor;
				return;
			}

			o_FragColor = mix(fillColor, strokeColor, strokeAlpha);
		}
	)";
}

namespace DGL
{
	/// @brief Compute a unique hash for a given blend mode.
	///
	/// This function computes a unique hash value for a BlendMode
	/// by combining the individual properties of the blend mode.
	/// This hash can be used to identify batches of shapes
	/// that share the same blend mode.
	///
	/// @param blendMode The blend mode to compute the hash for.
	/// @return A unique hash representing the blend mode.
	[[nodiscard]] constexpr size_t ComputeBlendModeHash(const BlendMode& blendMode)
	{
		size_t hash = 0;
		hash ^= static_cast<size_t>(blendMode.SourceColorFactor) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(blendMode.DestinationColorFactor) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(blendMode.SourceAlphaFactor) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(blendMode.DestinationAlphaFactor) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(blendMode.ColorEquation) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(blendMode.AlphaEquation) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}

	/// @brief Compute a unique hash for a given clipping rectangle.
	///
	/// This function computes a unique hash value for a ClipRect
	/// by combining its boundary properties. If the ClipRect
	/// is not clipped, it returns a hash of zero.
	/// This hash can be used to identify batches of shapes
	/// that share the same clipping rectangle.
	///
	/// @param clipRect The clipping rectangle to compute the hash for.
	/// @return A unique hash representing the clipping rectangle.
	[[nodiscard]] constexpr size_t ComputeClipRectHash(const ClipRect& clipRect)
	{
		if (not clipRect.IsClipped())
		{
			return 0;
		}

		const Math::IntBoundary& boundary = clipRect.UnwrapBoundary();

		size_t hash = 0;
		hash ^= static_cast<size_t>(boundary.Left) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(boundary.Top) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(boundary.Width) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(boundary.Height) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}

	template <typename Vertex>
	[[nodiscard]] constexpr HybridRendererBatchKey ComputeBatchKey(const HybridRendererBatch<Vertex>& batch)
	{
		size_t hash = 0;
		hash ^= ComputeBlendModeHash(batch.BlendMode) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= ComputeClipRectHash(batch.ClipRect) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}

	template <typename SDFVertex>
	[[nodiscard]] constexpr HybridRendererBatchKey ComputeBatchKey(const HybridSDFRendererBatch<SDFVertex>& batch)
	{
		size_t hash = 0;
		hash ^= ComputeBlendModeHash(batch.BlendMode) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= ComputeClipRectHash(batch.ClipRect) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(batch.IsFillEnabled) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(batch.IsStrokeEnabled) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}

	template <typename Batch>
	void MergeOrCreateBatch(std::unordered_map<HybridRendererBatchKey, Batch>& batchMap, const Batch& submission)
	{
		const HybridRendererBatchKey batchKey = ComputeBatchKey(submission);
		const auto it = batchMap.find(batchKey);
		if (it != batchMap.end())
		{
			Batch& batch = it->second;

			for (const uint32_t index : submission.Indices)
			{
				batch.Indices.emplace_back(static_cast<uint32_t>(batch.Vertices.size()) + index);
			}

			batch.Vertices.append_range(submission.Vertices);
		}
		else
		{
			batchMap.emplace(batchKey, submission);
		}
	}

	template <typename Batch>
	struct ReadOnlyChunk
	{
		using VertexType = std::add_const_t<typename Batch::VertexType>;

		const Batch* Original;
		std::span<VertexType>		Vertices;
		std::span<const uint32_t>	Indices;
	};

	template <typename Batch, std::invocable<ReadOnlyChunk<Batch>> Callback>
	void Chunked(
		const std::unordered_map<HybridRendererBatchKey, Batch>& batches,
		const size_t vertexCapacity,
		const size_t indexCapacity,
		Callback&& callback
	) {
		for (const auto& [batchKey, batch] : batches)
		{
			const size_t totalVertices = batch.Vertices.size();
			const size_t totalIndices = batch.Indices.size();

			size_t vertexOffset = 0;
			size_t indexOffset = 0;

			while (indexOffset < totalIndices)
			{
				const size_t verticesInThisChunk = std::min(totalVertices - vertexOffset, vertexCapacity);
				const size_t indicesInThisChunk = std::min(totalIndices - indexOffset, indexCapacity);

				ReadOnlyChunk<Batch> chunk{
					.Original = &batch,
					.Vertices = std::span(&batch.Vertices[vertexOffset], verticesInThisChunk),
					.Indices = std::span(&batch.Indices[indexOffset], indicesInThisChunk),
				};

				callback(std::move(chunk));
				vertexOffset += verticesInThisChunk;
				indexOffset += indicesInThisChunk;
			}
		}
	}

	static constexpr auto blendFactorToGlId = [](const BlendMode::Factor blendFactor) -> GLenum
		{
			switch (blendFactor)
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
		};

	static constexpr auto blendEquationToGlId = [](const BlendMode::Equation blendEquation)->GLenum
		{
			switch (blendEquation)
			{
			case BlendMode::Equation::Add:					return GL_FUNC_ADD;
			case BlendMode::Equation::Subtract:				return GL_FUNC_SUBTRACT;
			case BlendMode::Equation::ReverseSubtract:		return GL_FUNC_REVERSE_SUBTRACT;
			case BlendMode::Equation::Min:					return GL_MIN;
			case BlendMode::Equation::Max:					return GL_MAX;
			default:										ThrowError("Unknown BlendMode::Equation");
			}
		};

	auto& GetBlendModeActivator()
	{
		static auto activator = [currentBlendMode = std::optional<BlendMode>{}](const BlendMode& blendMode) mutable {
			if (currentBlendMode != blendMode) {
				glBlendFuncSeparate(blendFactorToGlId(blendMode.SourceColorFactor), blendFactorToGlId(blendMode.DestinationColorFactor), blendFactorToGlId(blendMode.SourceAlphaFactor), blendFactorToGlId(blendMode.DestinationAlphaFactor));
				glBlendEquationSeparate(blendEquationToGlId(blendMode.ColorEquation), blendEquationToGlId(blendMode.AlphaEquation));
				currentBlendMode = blendMode;
			}
			};

		return activator;
	}

	auto& GetClipRectActivator()
	{
		static auto activator = [currentClipRect = std::optional<ClipRect>{}](const ClipRect& clipRect) {
			if (currentClipRect != clipRect)
			{
				if (clipRect.IsClipped())
				{
					const Math::IntBoundary& clipBoundary = clipRect.UnwrapBoundary();
					glEnable(GL_SCISSOR_TEST);
					glScissor(static_cast<GLint>(clipBoundary.Left), static_cast<GLint>(clipBoundary.Top), static_cast<GLsizei>(clipBoundary.Width), static_cast<GLsizei>(clipBoundary.Height));
				}
				else
				{
					glDisable(GL_SCISSOR_TEST);
				}
			}
			};

		return activator;
	}

	std::unique_ptr<HybridRenderer> HybridRenderer::Create()
	{
		return std::unique_ptr<HybridRenderer>(
			new HybridRenderer(
				CreateGenericRenderingProperties(),
				CreateSDFCircleRenderingProperties(),
				CreateSDFRoundedRectRenderingProperties()
			)
		);
	}

	HybridRenderer::~HybridRenderer()
	{
		// TODO(Felix): Release resources
	}

	void HybridRenderer::SetViewport(const Math::FloatBoundary& viewport)
	{
		m_Viewport = viewport;
		m_ProjectionMatrix = Math::Matrix4x4::Orthographic(viewport, -1.0f, 1.0f);
	}

	void HybridRenderer::BeginDraw()
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);

		m_DepthProvider.Reset();
	}

	void HybridRenderer::EndDraw()
	{
		Flush();
	}

	void HybridRenderer::RenderRectangle(const Math::FloatBoundary& boundary, const Math::Float4& fillColor, const Math::Float4& strokeColor, const float strokeWeight, const StrokeProperties& strokeProperties, const RenderingProperties& properties)
	{
		HybridRendererBatch<HybridRendererGenericVertex> submission = {
			.BlendMode = properties.BlendMode,
			.ClipRect = properties.ClippingRect,
		};

		auto [left, top, width, height] = boundary;
		left = std::min(left, left + width);
		top = std::min(top, top + height);
		width = std::abs(width);
		height = std::abs(height);

		const std::array points = { Math::Float2{ left, top }, Math::Float2{ left + width, top }, Math::Float2{ left + width, top + height }, Math::Float2{ left, top + height }, };
		const std::array colors = { strokeColor, strokeColor, strokeColor, strokeColor };

		// Generate fill vertices & indices
		if (properties.IsFillEnabled)
		{
			const float fillDepth = m_DepthProvider.GetAndIncrement();
			for (const Math::Float2& point : points)
			{
				submission.Vertices.emplace_back(HybridRendererGenericVertex{
					.Position = properties.ModelMatrix.TransformPoint(Math::Float3{ point.X, point.Y, fillDepth }),
					.Color = fillColor,
				});
			}
		
			submission.Indices = { 0, 1, 2, 2, 3, 0 };
		}

		// Generate stroke vertices
		if (properties.IsStrokeEnabled)
		{
			const float strokeDepth = m_DepthProvider.GetAndIncrement();
			const Stroke stroke = m_Stroker.GenerateStroke(points, colors, strokeWeight, strokeProperties.MiterLimit, strokeProperties.Alignment, strokeProperties.StrokeCap, strokeProperties.StrokeJoin, strokeProperties.IsClosed);

			const size_t baseIndex = submission.Vertices.size();
			// Convert the outline to vertices and indices
			for (const auto& [position, color] : stroke.Vertices)
			{
				submission.Vertices.emplace_back(HybridRendererGenericVertex{
					.Position = Math::Float3{ position.X, position.Y, strokeDepth },
					.Color = color,
				});
			}

			for (const uint32_t index : stroke.Indices)
			{
				submission.Indices.emplace_back(baseIndex + index);
			}
		}

		// Print out the number of vertices and indices for debugging
		Info(std::format("Rectangle Submission: Vertices = {}, Indices = {}", submission.Vertices.size(), submission.Indices.size()));

		// Submit the batch
		SubmitBatch(submission);
	}

	void HybridRenderer::RenderRoundedRectangle(const Math::FloatBoundary& boundary, const Math::BorderRadius& cornerRadii, const Math::Float4& fillColor, const Math::Float4& strokeColor, float strokeWeight, const RenderingProperties& properties)
	{
		// Create an SDF rounded rectangle batch submission
		HybridSDFRendererBatch<HybridRendererSDFRoundedRectVertex> submission = {
			.BlendMode = properties.BlendMode,
			.ClipRect = properties.ClippingRect,
			.IsFillEnabled = properties.IsFillEnabled,
			.IsStrokeEnabled = properties.IsStrokeEnabled,
		};

		// Get the depth for this shape
		const float depth = m_DepthProvider.GetAndIncrement();

		// Transform the rectangle corners using the model matrix
		auto [left, top, width, height] = boundary;
		left = std::min(left, left + width);
		top = std::min(top, top + height);
		width = std::abs(width);
		height = std::abs(height);

		const Math::Float3 topLeft = properties.ModelMatrix.TransformPoint(Math::Float3{ left, top, depth });
		const Math::Float3 topRight = properties.ModelMatrix.TransformPoint(Math::Float3{ left + width, top, depth });
		const Math::Float3 bottomRight = properties.ModelMatrix.TransformPoint(Math::Float3{ left + width, top + height, depth });
		const Math::Float3 bottomLeft = properties.ModelMatrix.TransformPoint(Math::Float3{ left, top + height, depth });

		// Create vertices for the rounded rectangle
		submission.Vertices = {
			HybridRendererSDFRoundedRectVertex{.WorldPosition = topLeft,		.LocalPosition = { -1.0f, -1.0f },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .RectSize = { width, height }, .CornerRadius = cornerRadii.TopLeft, },
			HybridRendererSDFRoundedRectVertex{.WorldPosition = topRight,		.LocalPosition = {  1.0f, -1.0f },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .RectSize = { width, height }, .CornerRadius = cornerRadii.TopRight },
			HybridRendererSDFRoundedRectVertex{.WorldPosition = bottomRight,	.LocalPosition = {  1.0f,  1.0f },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .RectSize = { width, height }, .CornerRadius = cornerRadii.BottomRight },
			HybridRendererSDFRoundedRectVertex{.WorldPosition = bottomLeft,		.LocalPosition = { -1.0f,  1.0f },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .RectSize = { width, height }, .CornerRadius = cornerRadii.BottomLeft },
		};

		// Define the indices for two triangles that make up the rounded rectangle
		submission.Indices = { 0, 1, 2, 2, 3, 0 };

		// Submit the batch
		SubmitBatch(submission);
	}

	void HybridRenderer::RenderEllipse(const Math::Float2& center, const Math::Radius& radius, const Math::Float4& fillColor, const Math::Float4& strokeColor, const float strokeWeight, const RenderingProperties& properties)
	{
		// Create an SDF circle batch submission
		HybridSDFRendererBatch<HybridRendererSDFCircleVertex> submission = {
			.BlendMode = properties.BlendMode,
			.ClipRect = properties.ClippingRect,
			.IsFillEnabled = properties.IsFillEnabled,
			.IsStrokeEnabled = properties.IsStrokeEnabled,
		};

		// Get the depth for this shape
		const float depth = m_DepthProvider.GetAndIncrement();

		Math::Float2 transformedRadius = properties.ModelMatrix.TransformVector(Math::Float2{ radius.X, radius.Y });
		transformedRadius.X = std::abs(transformedRadius.X);
		transformedRadius.Y = std::abs(transformedRadius.Y);

		// Transform the center point using the model matrix
		const Math::Float3 transformedCenter = properties.ModelMatrix.TransformPoint(Math::Float3{ center.X, center.Y, depth });

		const Math::Float2 localTopLeft = { -transformedRadius.X, -transformedRadius.Y };
		const Math::Float2 localTopRight = { transformedRadius.X, -transformedRadius.Y };
		const Math::Float2 localBottomRight = { transformedRadius.X,  transformedRadius.Y };
		const Math::Float2 localBottomLeft = { -transformedRadius.X,  transformedRadius.Y };

		// Create vertices for the circle quad
		submission.Vertices = {
			HybridRendererSDFCircleVertex{.WorldPosition = transformedCenter + Math::Float3{ localTopLeft, 0.0f },		.LocalPosition = { -1.0f, -1.0f, },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .CircleSize = { transformedRadius.X * 2.0f, transformedRadius.Y * 2.0f }, },
			HybridRendererSDFCircleVertex{.WorldPosition = transformedCenter + Math::Float3{ localTopRight, 0.0f },	.LocalPosition = {  1.0f, -1.0f, },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .CircleSize = { transformedRadius.X * 2.0f, transformedRadius.Y * 2.0f }, },
			HybridRendererSDFCircleVertex{.WorldPosition = transformedCenter + Math::Float3{ localBottomRight, 0.0f },	.LocalPosition = {  1.0f,  1.0f, },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .CircleSize = { transformedRadius.X * 2.0f, transformedRadius.Y * 2.0f }, },
			HybridRendererSDFCircleVertex{.WorldPosition = transformedCenter + Math::Float3{ localBottomLeft, 0.0f },	.LocalPosition = { -1.0f,  1.0f, },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .CircleSize = { transformedRadius.X * 2.0f, transformedRadius.Y * 2.0f }, },
		};

		// Define the indices for two triangles that make up the quad
		submission.Indices = { 0, 1, 2, 2, 3, 0 };

		// Submit the batch
		SubmitBatch(submission);
	}

	HybridRenderer::GenericRenderingProperties HybridRenderer::CreateGenericRenderingProperties()
	{
		constexpr size_t GenericVertexBufferCapacity = 10'000;
		constexpr size_t GenericElementBufferCapacity = 30'000;

		GLuint genericVertexBufferId = 0;
		glCreateBuffers(1, &genericVertexBufferId);
		glNamedBufferStorage(genericVertexBufferId, sizeof(HybridRendererGenericVertex) * GenericVertexBufferCapacity, nullptr, GL_DYNAMIC_STORAGE_BIT);

		GLuint genericElementBufferId = 0;
		glCreateBuffers(1, &genericElementBufferId);
		glNamedBufferStorage(genericElementBufferId, sizeof(uint32_t) * GenericElementBufferCapacity, nullptr, GL_DYNAMIC_STORAGE_BIT);

		GLuint genericVertexArrayId = 0;
		glCreateVertexArrays(1, &genericVertexArrayId);
		glVertexArrayVertexBuffer(genericVertexArrayId, 0, genericVertexBufferId, 0, sizeof(HybridRendererGenericVertex));
		glVertexArrayElementBuffer(genericVertexArrayId, genericElementBufferId);

		glEnableVertexArrayAttrib(genericVertexArrayId, 0); // Position
		glVertexArrayAttribFormat(genericVertexArrayId, 0, 3, GL_FLOAT, GL_FALSE, offsetof(HybridRendererGenericVertex, Position));
		glVertexArrayAttribBinding(genericVertexArrayId, 0, 0);

		glEnableVertexArrayAttrib(genericVertexArrayId, 1); // Color
		glVertexArrayAttribFormat(genericVertexArrayId, 1, 4, GL_FLOAT, GL_FALSE, offsetof(HybridRendererGenericVertex, Color));
		glVertexArrayAttribBinding(genericVertexArrayId, 1, 0);

		auto genericShader = Shader::CreateFromSource(GENERIC_VERTEX_SOURCE, GENERIC_FRAGMENT_SOURCE);

		return {
			.VertexArrayId = genericVertexArrayId,
			.VertexBufferId = genericVertexBufferId,
			.ElementBufferId = genericElementBufferId,
			.VertexBufferCapacity = GenericVertexBufferCapacity,
			.ElementBufferCapacity = GenericElementBufferCapacity,
			.Shader = std::move(genericShader),
		};
	}

	HybridRenderer::SDFCircleRenderingProperties HybridRenderer::CreateSDFCircleRenderingProperties()
	{
		constexpr size_t SDFCircleVertexBufferCapacity = 10'000;
		constexpr size_t SDFCircleElementBufferCapacity = 30'000;

		GLuint sdfCircleVertexBufferId = 0;
		glCreateBuffers(1, &sdfCircleVertexBufferId);
		glNamedBufferStorage(sdfCircleVertexBufferId, sizeof(HybridRendererSDFCircleVertex) * SDFCircleVertexBufferCapacity, nullptr, GL_DYNAMIC_STORAGE_BIT);

		GLuint sdfCircleElementBufferId = 0;
		glCreateBuffers(1, &sdfCircleElementBufferId);
		glNamedBufferStorage(sdfCircleElementBufferId, sizeof(uint32_t) * SDFCircleElementBufferCapacity, nullptr, GL_DYNAMIC_STORAGE_BIT);

		GLuint sdfCircleVertexArrayId = 0;
		glCreateVertexArrays(1, &sdfCircleVertexArrayId);
		glVertexArrayVertexBuffer(sdfCircleVertexArrayId, 0, sdfCircleVertexBufferId, 0, sizeof(HybridRendererSDFCircleVertex));
		glVertexArrayElementBuffer(sdfCircleVertexArrayId, sdfCircleElementBufferId);

		glEnableVertexArrayAttrib(sdfCircleVertexArrayId, 0); // WorldPosition
		glVertexArrayAttribFormat(sdfCircleVertexArrayId, 0, 3, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFCircleVertex, WorldPosition));
		glVertexArrayAttribBinding(sdfCircleVertexArrayId, 0, 0);
		glEnableVertexArrayAttrib(sdfCircleVertexArrayId, 1); // LocalPosition
		glVertexArrayAttribFormat(sdfCircleVertexArrayId, 1, 2, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFCircleVertex, LocalPosition));
		glVertexArrayAttribBinding(sdfCircleVertexArrayId, 1, 0);
		glEnableVertexArrayAttrib(sdfCircleVertexArrayId, 2); // FillColor
		glVertexArrayAttribFormat(sdfCircleVertexArrayId, 2, 4, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFCircleVertex, FillColor));
		glVertexArrayAttribBinding(sdfCircleVertexArrayId, 2, 0);
		glEnableVertexArrayAttrib(sdfCircleVertexArrayId, 3); // StrokeColor
		glVertexArrayAttribFormat(sdfCircleVertexArrayId, 3, 4, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFCircleVertex, StrokeColor));
		glVertexArrayAttribBinding(sdfCircleVertexArrayId, 3, 0);
		glEnableVertexArrayAttrib(sdfCircleVertexArrayId, 4); // StrokeWeight
		glVertexArrayAttribFormat(sdfCircleVertexArrayId, 4, 1, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFCircleVertex, StrokeWeight));
		glVertexArrayAttribBinding(sdfCircleVertexArrayId, 4, 0);
		glEnableVertexArrayAttrib(sdfCircleVertexArrayId, 5); // CircleSize
		glVertexArrayAttribFormat(sdfCircleVertexArrayId, 5, 2, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFCircleVertex, CircleSize));
		glVertexArrayAttribBinding(sdfCircleVertexArrayId, 5, 0);

		std::unique_ptr<Shader> sdfCircleShader = Shader::CreateFromSource(SDF_CIRCLE_VERTEX_SOURCE, SDF_CIRCLE_FRAGMENT_SOURCE);

		return {
			.VertexArrayId = sdfCircleVertexArrayId,
			.VertexBufferId = sdfCircleVertexBufferId,
			.ElementBufferId = sdfCircleElementBufferId,
			.VertexBufferCapacity = SDFCircleVertexBufferCapacity,
			.ElementBufferCapacity = SDFCircleElementBufferCapacity,
			.Shader = std::move(sdfCircleShader),
		};
	}

	HybridRenderer::SDFRoundedRectRenderingProperties HybridRenderer::CreateSDFRoundedRectRenderingProperties()
	{
		constexpr size_t SDFRoundedRectVertexBufferCapacity = 10'000;
		constexpr size_t SDFRoundedRectElementBufferCapacity = 30'000;

		GLuint sdfRoundedRectVertexBuffer = 0;
		glCreateBuffers(1, &sdfRoundedRectVertexBuffer);
		glNamedBufferStorage(sdfRoundedRectVertexBuffer, sizeof(HybridRendererSDFRoundedRectVertex) * SDFRoundedRectVertexBufferCapacity, nullptr, GL_DYNAMIC_STORAGE_BIT);

		GLuint sdfRoundedRectElementBuffer = 0;
		glCreateBuffers(1, &sdfRoundedRectElementBuffer);
		glNamedBufferStorage(sdfRoundedRectElementBuffer, sizeof(uint32_t) * SDFRoundedRectElementBufferCapacity, nullptr, GL_DYNAMIC_STORAGE_BIT);

		GLuint sdfRoundedRectVertexArray = 0;
		glCreateVertexArrays(1, &sdfRoundedRectVertexArray);
		glVertexArrayVertexBuffer(sdfRoundedRectVertexArray, 0, sdfRoundedRectVertexBuffer, 0, sizeof(HybridRendererSDFRoundedRectVertex));
		glVertexArrayElementBuffer(sdfRoundedRectVertexArray, sdfRoundedRectElementBuffer);

		glEnableVertexArrayAttrib(sdfRoundedRectVertexArray, 0); // WorldPosition
		glVertexArrayAttribFormat(sdfRoundedRectVertexArray, 0, 3, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFRoundedRectVertex, WorldPosition));
		glVertexArrayAttribBinding(sdfRoundedRectVertexArray, 0, 0);

		glEnableVertexArrayAttrib(sdfRoundedRectVertexArray, 1); // LocalPosition
		glVertexArrayAttribFormat(sdfRoundedRectVertexArray, 1, 2, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFRoundedRectVertex, LocalPosition));
		glVertexArrayAttribBinding(sdfRoundedRectVertexArray, 1, 0);

		glEnableVertexArrayAttrib(sdfRoundedRectVertexArray, 2); // FillColor
		glVertexArrayAttribFormat(sdfRoundedRectVertexArray, 2, 4, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFRoundedRectVertex, FillColor));
		glVertexArrayAttribBinding(sdfRoundedRectVertexArray, 2, 0);

		glEnableVertexArrayAttrib(sdfRoundedRectVertexArray, 3); // StrokeColor
		glVertexArrayAttribFormat(sdfRoundedRectVertexArray, 3, 4, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFRoundedRectVertex, StrokeColor));
		glVertexArrayAttribBinding(sdfRoundedRectVertexArray, 3, 0);

		glEnableVertexArrayAttrib(sdfRoundedRectVertexArray, 4); // StrokeWeight
		glVertexArrayAttribFormat(sdfRoundedRectVertexArray, 4, 1, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFRoundedRectVertex, StrokeWeight));
		glVertexArrayAttribBinding(sdfRoundedRectVertexArray, 4, 0);

		glEnableVertexArrayAttrib(sdfRoundedRectVertexArray, 5); // RectSize
		glVertexArrayAttribFormat(sdfRoundedRectVertexArray, 5, 2, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFRoundedRectVertex, RectSize));
		glVertexArrayAttribBinding(sdfRoundedRectVertexArray, 5, 0);

		glEnableVertexArrayAttrib(sdfRoundedRectVertexArray, 6); // CornerRadius
		glVertexArrayAttribFormat(sdfRoundedRectVertexArray, 6, 1, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFRoundedRectVertex, CornerRadius));
		glVertexArrayAttribBinding(sdfRoundedRectVertexArray, 6, 0);

		std::unique_ptr<Shader> sdfRoundedRectShader = Shader::CreateFromSource(SDF_ROUNDED_RECT_VERTEX_SOURCE, SDF_ROUNDED_RECT_FRAGMENT_SOURCE);

		return {
			.VertexArrayId = sdfRoundedRectVertexArray,
			.VertexBufferId = sdfRoundedRectVertexBuffer,
			.ElementBufferId = sdfRoundedRectElementBuffer,
			.VertexBufferCapacity = SDFRoundedRectVertexBufferCapacity,
			.ElementBufferCapacity = SDFRoundedRectElementBufferCapacity,
			.Shader = std::move(sdfRoundedRectShader),
		};
	}

	HybridRenderer::HybridRenderer(
		GenericRenderingProperties&& genericProperties,
		SDFCircleRenderingProperties&& sdfCircleProperties,
		SDFRoundedRectRenderingProperties&& sdfRoundedRectProperties
	) : m_GenericProperties(std::move(genericProperties)),
		m_SDFCircleProperties(std::move(sdfCircleProperties)),
		m_SDFRoundedRectProperties(std::move(sdfRoundedRectProperties)),
		m_DepthProvider(0.0f, 1.0f / 20'000.0f)
	{
	}

	void HybridRenderer::Flush()
	{
		FlushGenericBatch();
		FlushSDFCircleBatch();
		FlushSDFRoundedRectBatch();
	}

	void HybridRenderer::FlushGenericBatch()
	{
		// Early our if there's nothing to flush
		if (m_GenericBatches.empty())
		{
			return;
		}

		// Unfortunately, we can not assume that the batches are small enough to fit into the
		// vertex buffers / element buffers. In order to submit all data correctly, we need to
		// split batches that exceed the buffer capacities into smaller sub-batches (chunks).

		glBindVertexArray(m_GenericProperties.VertexArrayId);
		glUseProgram(m_GenericProperties.Shader->GetShaderId());
		m_GenericProperties.Shader->UploadFloatMatrix4x4("u_ProjectionMatrix", m_ProjectionMatrix.GetData());

		static auto& activateBlendMode = GetBlendModeActivator();
		static auto& activateClipRect = GetClipRectActivator();

		Chunked(
			m_GenericBatches,
			m_GenericProperties.VertexBufferCapacity,
			m_GenericProperties.ElementBufferCapacity,
			[this](ReadOnlyChunk<HybridRendererBatch<HybridRendererGenericVertex>>&& chunk)
			{
				// Maybe we can do this once at the start?
				activateBlendMode(chunk.Original->BlendMode);
				activateClipRect(chunk.Original->ClipRect);

				// Upload the data to the GPU
				glNamedBufferSubData(m_GenericProperties.VertexBufferId, 0, chunk.Vertices.size_bytes(), chunk.Vertices.data());
				glNamedBufferSubData(m_GenericProperties.ElementBufferId, 0, chunk.Indices.size_bytes(), chunk.Indices.data());

				// Draw the chunk
				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(chunk.Indices.size()), GL_UNSIGNED_INT, nullptr); //!< Maybe get offset pointer correct?
			}
		);

		// Clear all generic batches after flushing
		m_GenericBatches.clear();
	}

	void HybridRenderer::FlushSDFCircleBatch()
	{
		// Early our if there's nothing to flush
		if (m_SDFCircleBatches.empty())
		{
			return;
		}

		static auto& blendModeActivator = GetBlendModeActivator();
		static auto& clipRectActivator = GetClipRectActivator();

		// Unfortunately, we can not assume that the batches are small enough to fit into the
		// vertex buffers / element buffers. In order to submit all data correctly, we need to
		// split batches that exceed the buffer capacities into smaller sub-batches (chunks).
		glBindVertexArray(m_SDFCircleProperties.VertexArrayId);
		glUseProgram(m_SDFCircleProperties.Shader->GetShaderId());
		m_SDFCircleProperties.Shader->UploadFloatMatrix4x4("u_ProjectionMatrix", m_ProjectionMatrix.GetData());

		Chunked(
			m_SDFCircleBatches,
			m_SDFCircleProperties.VertexBufferCapacity,
			m_SDFCircleProperties.ElementBufferCapacity,
			[this](ReadOnlyChunk<HybridSDFRendererBatch<HybridRendererSDFCircleVertex>>&& chunk)
			{
				// Maybe we can do this once at the start?
				blendModeActivator(chunk.Original->BlendMode);
				clipRectActivator(chunk.Original->ClipRect);

				m_SDFCircleProperties.Shader->UploadInt1("u_IsFillEnabled", static_cast<int>(chunk.Original->IsFillEnabled));
				m_SDFCircleProperties.Shader->UploadInt1("u_IsStrokeEnabled", static_cast<int>(chunk.Original->IsStrokeEnabled));

				// Upload the data to the GPU
				glNamedBufferSubData(m_SDFCircleProperties.VertexBufferId, 0, chunk.Vertices.size_bytes(), chunk.Vertices.data());
				glNamedBufferSubData(m_SDFCircleProperties.ElementBufferId, 0, chunk.Indices.size_bytes(), chunk.Indices.data());

				// Draw the chunk
				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(chunk.Indices.size()), GL_UNSIGNED_INT, nullptr); //!< Maybe get offset pointer correct?
			}
		);

		// Clear all SDF circle batches after flushing
		m_SDFCircleBatches.clear();
	}

	void HybridRenderer::FlushSDFRoundedRectBatch()
	{
		// Early our if there's nothing to flush
		if (m_SDFRoundedRectBatches.empty())
		{
			return;
		}

		static auto& blendModeActivator = GetBlendModeActivator();
		static const auto clipRectActivator = GetClipRectActivator();

		// Unfortunately, we can not assume that the batches are small enough to fit into the
		// vertex buffers / element buffers. In order to submit all data correctly, we need to
		// split batches that exceed the buffer capacities into smaller sub-batches (chunks).

		glBindVertexArray(m_SDFRoundedRectProperties.VertexArrayId);
		glUseProgram(m_SDFRoundedRectProperties.Shader->GetShaderId());
		m_SDFRoundedRectProperties.Shader->UploadFloatMatrix4x4("u_ProjectionMatrix", m_ProjectionMatrix.GetData());

		Chunked(
			m_SDFRoundedRectBatches,
			m_SDFRoundedRectProperties.VertexBufferCapacity,
			m_SDFRoundedRectProperties.ElementBufferCapacity,
			[this](ReadOnlyChunk<HybridSDFRendererBatch<HybridRendererSDFRoundedRectVertex>>&& chunk)
			{
				// Maybe we can do this once at the start?
				blendModeActivator(chunk.Original->BlendMode);
				clipRectActivator(chunk.Original->ClipRect);

				m_SDFRoundedRectProperties.Shader->UploadInt1("u_IsFillEnabled", static_cast<int>(chunk.Original->IsFillEnabled));
				m_SDFRoundedRectProperties.Shader->UploadInt1("u_IsStrokeEnabled", static_cast<int>(chunk.Original->IsStrokeEnabled));

				// Upload the data to the GPU
				glNamedBufferSubData(m_SDFRoundedRectProperties.VertexBufferId, 0, chunk.Vertices.size_bytes(), chunk.Vertices.data());
				glNamedBufferSubData(m_SDFRoundedRectProperties.ElementBufferId, 0, chunk.Indices.size_bytes(), chunk.Indices.data());

				// Draw the chunk
				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(chunk.Indices.size()), GL_UNSIGNED_INT, nullptr); //!< Maybe get offset pointer correct?
			}
		);

		// Clear all SDF rounded-rectangle batches after flushing
		m_SDFRoundedRectBatches.clear();
	}

	void HybridRenderer::SubmitBatch(const HybridRendererBatch<HybridRendererGenericVertex>& submission)
	{
		MergeOrCreateBatch(m_GenericBatches, submission);
	}

	void HybridRenderer::SubmitBatch(const HybridSDFRendererBatch<HybridRendererSDFCircleVertex>& submission)
	{
		MergeOrCreateBatch(m_SDFCircleBatches, submission);
	}

	void HybridRenderer::SubmitBatch(const HybridSDFRendererBatch<HybridRendererSDFRoundedRectVertex>& submission)
	{
		MergeOrCreateBatch(m_SDFRoundedRectBatches, submission);
	}
}