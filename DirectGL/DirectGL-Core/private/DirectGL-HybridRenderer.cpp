module;

#include <glad/gl.h>
#include <functional>
#include <format>
#include <span>
#include <optional>
#include <chrono>

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
		
		float sdfEllipse(vec2 p, vec2 radius)
		{
		    vec2 q = p / radius;
		    return length(q) - 1.0;
		}

		void main()
		{
			vec2 radius = vec2(1.0);
			float dist = sdfEllipse(v_LocalPosition, radius);
			float antiAlias = fwidth(dist);

			// Compute the conversion factor from pixels to local space
			//float halfStrokeWeightPx = v_StrokeWeight * 0.5;
			//float halfStrokeLocal = halfStrokeWeightPx * antiAlias;

			vec2 pixelToLocal = 2.0 / v_CircleSize;
			float pxToLocal = 0.5 * (pixelToLocal.x + pixelToLocal.y);
			float halfStrokeLocal = v_StrokeWeight * 0.5 * pxToLocal;

			// Define inner and outer distances for fill and stroke
			float inner = -halfStrokeLocal;
			float outer = 0.0;

			// Compute the fill color with anti-aliasing
			float fillFactor	= smoothstep(inner - antiAlias, inner + antiAlias, dist);
			float fillMask		= 1.0 - fillFactor;
			float strokeFactor	= smoothstep(outer + antiAlias, outer - antiAlias, dist);
			float strokeMask	= strokeFactor;

			vec4 fillColor = vec4(v_FillColor.rgb, v_FillColor.a * fillMask);
			vec4 strokeColor = vec4(v_StrokeColor.rgb, v_StrokeColor.a * strokeMask);
			vec4 combinedColor = fillColor + strokeColor * (1.0 - fillColor.a);

			// Combine fill and stroke colors
			o_FragColor = vec4(combinedColor.rgba);
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
		
		float sdfRoundedBox( in vec2 p, in vec2 b, in float r ) {
			vec2 q = abs(p)-b+r;
			return min(max(q.x,q.y),0.0) + length(max(q,0.0)) - r;
		}
		
		void main()
		{
			vec2 halfSize = v_RectSize * 0.5;
			float normalizedBorderRadius = max(v_BorderRadius / min(halfSize.x, halfSize.y), 0.0);
			
			float dist = sdfRoundedBox(v_LocalPosition, vec2(1.0), normalizedBorderRadius);
			float antiAlias = fwidth(dist);

			// Compute the conversion factor from pixels to local space
			vec2 pixelToLocal = 2.0 / v_RectSize;
			float pxToLocal = 0.5 * (pixelToLocal.x + pixelToLocal.y);
			float halfStrokeLocal = v_StrokeWeight * 0.5 * pxToLocal;

			// Define inner and outer distances for fill and stroke
			float inner = -halfStrokeLocal;
			float outer = 0.0;

			// Compute the fill color with anti-aliasing
			float fillFactor	= smoothstep(inner - antiAlias, inner + antiAlias, dist);
			float fillMask		= 1.0 - fillFactor;
			float strokeFactor	= smoothstep(outer + antiAlias, outer - antiAlias, dist);
			float strokeMask	= strokeFactor;

			vec4 fillColor = vec4(v_FillColor.rgb, v_FillColor.a * fillMask);
			vec4 strokeColor = vec4(v_StrokeColor.rgb, v_StrokeColor.a * strokeMask);
			vec4 combinedColor = fillColor + strokeColor * (1.0 - fillColor.a);

			// Combine fill and stroke colors
			o_FragColor = vec4(combinedColor);
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
		hash ^= static_cast<size_t>(blendMode.SourceColorFactor)			+ 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(blendMode.DestinationColorFactor)		+ 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(blendMode.SourceAlphaFactor)			+ 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(blendMode.DestinationAlphaFactor)		+ 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(blendMode.ColorEquation)				+ 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= static_cast<size_t>(blendMode.AlphaEquation)				+ 0x9e3779b9 + (hash << 6) + (hash >> 2);
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

	/// @brief Compute a unique key for a batch based on its rendering properties.
	///
	/// This function computes a unique key for a batch of shapes
	/// based on its rendering properties, specifically the blend mode
	/// and clipping rectangle. This key is used to group shapes
	/// with similar properties together for efficient rendering.
	///
	/// @param blendMode The blend mode of the batch.
	/// @param clipRect The clipping rectangle of the batch.
	/// @return A unique key representing the batch.
	[[nodiscard]] static HybridRendererBatchKey ComputeBatchKey(const BlendMode& blendMode, const ClipRect& clipRect)
	{
		return ComputeBlendModeHash(blendMode) ^ (ComputeClipRectHash(clipRect) << 1);
	}

	/// @brief Merge a submission batch into an existing batch.
	///
	/// This function merges the contents of a submission batch
	/// into an existing batch. It appends the vertices and
	/// indices from the submission to the existing batch,
	/// adjusting the indices accordingly to account for
	/// the new vertex positions.
	///
	/// @param batch The existing batch to merge into.
	/// @param submission The submission batch to merge from.
	/// @tparam Vertex The type of vertex used in the batches.
	template <typename Vertex>
	void MergeBatch(HybridRendererBatch<Vertex>& batch, const HybridRendererBatch<Vertex>& submission)
	{
		// Store the offset where the new vertices will be added
		const size_t indexOffset = batch.Vertices.size();

		// Append all vertices to the batch
		batch.Vertices.append_range(submission.Vertices);

		// Append all indices to the
		batch.Indices.reserve(batch.Indices.size() + submission.Indices.size());
		for (const uint32_t index : submission.Indices)
		{
			batch.Indices.push_back(static_cast<uint32_t>(indexOffset) + index);
		}
	}

	/// @brief Merge a submission batch into the batch map or create a new batch.
	///
	/// This function checks if a batch with the same rendering properties
	/// as the submission already exists in the batch map. If it does,
	/// it merges the submission into the existing batch. If not,
	/// it creates a new batch in the map with the submission.
	///
	/// @param batchMap The map of existing batches.
	/// @param submission The submission batch to merge or create.
	/// @tparam Vertex The type of vertex used in the batches.
	template <typename Vertex>
	void MergeOrCreateBatch(std::map<HybridRendererBatchKey, HybridRendererBatch<Vertex>>& batchMap, const HybridRendererBatch<Vertex>& submission)
	{
		const HybridRendererBatchKey batchKey = ComputeBatchKey(submission.BlendMode, submission.ClipRect);
		const auto it = batchMap.find(batchKey);
		if (it != batchMap.end())
		{
			MergeBatch(it->second, submission);
		}
		else
		{
			batchMap.emplace(batchKey, submission);
		}
	}

	template <typename Vertex>
	struct ReadOnlyChunk
	{
		const BlendMode*				BlendMode;
		const ClipRect*					ClipRect;
		std::span<const Vertex>			Vertices;
		std::span<const uint32_t>		Indices;
		size_t							IndexOffset;
	};

	template <typename Vertex, std::invocable<ReadOnlyChunk<Vertex>> Callback>
	void Chunked(
		const std::map<HybridRendererBatchKey, HybridRendererBatch<Vertex>>& batches,
		const size_t vertexCapacity,
		const size_t indexCapacity,
		Callback&& callback
	) {
		for (const auto& [batchKey, batch] : batches)
		{
			size_t totalVertices = batch.Vertices.size();
			size_t totalIndices = batch.Indices.size();
			size_t vertexOffset = 0;
			size_t indexOffset = 0;

			while (indexOffset < totalIndices)
			{
				const size_t verticesInThisChunk = std::min(totalVertices - vertexOffset, vertexCapacity);
				const size_t indicesInThisChunk = std::min(totalIndices - indexOffset, indexCapacity);

				ReadOnlyChunk<Vertex> chunk {
					.BlendMode		= &batch.BlendMode,
					.ClipRect		= &batch.ClipRect,
					.Vertices		= std::span(&batch.Vertices[vertexOffset], verticesInThisChunk),
					.Indices		= std::span(&batch.Indices[indexOffset], indicesInThisChunk),
					.IndexOffset	= vertexOffset,
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

	auto GetBlendModeActivator()
	{
		std::optional<BlendMode> currentBlendMode;

		return [&currentBlendMode](const BlendMode& blendMode) {
			if (currentBlendMode != blendMode) {
				glBlendFuncSeparate(blendFactorToGlId(blendMode.SourceColorFactor), blendFactorToGlId(blendMode.DestinationColorFactor), blendFactorToGlId(blendMode.SourceAlphaFactor), blendFactorToGlId(blendMode.DestinationAlphaFactor));
				glBlendEquationSeparate(blendEquationToGlId(blendMode.ColorEquation), blendEquationToGlId(blendMode.AlphaEquation));
			}
		};
	}

	auto GetClipRectActivator()
	{
		std::optional<ClipRect> currentClipRect;

		return [&currentClipRect](const ClipRect& clipRect) {
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
	}

	std::unique_ptr<HybridRenderer> HybridRenderer::Create()
	{
		constexpr size_t GenericVertexBufferCapacity	= 10'000;
		constexpr size_t GenericElementBufferCapacity	= 30'000;

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
		glEnableVertexArrayAttrib(sdfRoundedRectVertexArray, 6); // CornerRadii
		glVertexArrayAttribFormat(sdfRoundedRectVertexArray, 6, 1, GL_FLOAT, GL_FALSE, offsetof(HybridRendererSDFRoundedRectVertex, CornerRadii));
		glVertexArrayAttribBinding(sdfRoundedRectVertexArray, 6, 0);

		std::unique_ptr<Shader> sdfRoundedRectShader = Shader::CreateFromSource(SDF_ROUNDED_RECT_VERTEX_SOURCE, SDF_ROUNDED_RECT_FRAGMENT_SOURCE);

		return std::unique_ptr<HybridRenderer>(
			new HybridRenderer(
				genericVertexArrayId,
				genericVertexBufferId,
				genericElementBufferId,
				GenericVertexBufferCapacity,
				GenericElementBufferCapacity,
				std::move(genericShader),
				sdfCircleVertexArrayId,
				sdfCircleVertexBufferId,
				sdfCircleElementBufferId,
				SDFCircleVertexBufferCapacity,
				SDFCircleElementBufferCapacity,
				std::move(sdfCircleShader),
				sdfRoundedRectVertexArray,
				sdfRoundedRectVertexBuffer,
				sdfRoundedRectElementBuffer,
				SDFRoundedRectVertexBufferCapacity,
				SDFRoundedRectElementBufferCapacity,
				std::move(sdfRoundedRectShader)
			)
		);
	}

	HybridRenderer::~HybridRenderer()
	{
		glDeleteVertexArrays(1, &m_GenericVertexArrayId);
		glDeleteBuffers(1, &m_GenericVertexBufferId);
		glDeleteBuffers(1, &m_GenericElementBufferId);
	}

	void HybridRenderer::SetViewport(const Math::FloatBoundary& viewport)
	{
		m_ProjectionMatrix = Math::Matrix4x4::Orthographic(viewport, -1.0f, 1.0f);
	}

	void HybridRenderer::BeginDraw()
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_BLEND);

		m_DepthProvider.Reset();
	}

	void HybridRenderer::EndDraw()
	{
		Flush();
	}

	void HybridRenderer::FillRectangle(const Math::FloatBoundary& boundary, const Math::Float4& color, const RenderingProperties& properties)
	{
		// Create a generic batch submission for the rectangle
		HybridRendererBatch<HybridRendererGenericVertex> submission = {
			.BlendMode = properties.BlendMode,
			.ClipRect = properties.ClippingRect,
		};

		// Get the depth for this shape
		const float depth = m_DepthProvider.GetAndIncrement();

		// Define the four corners of the rectangle
		const auto [left, top, width, height] = boundary;
		const Math::Float3 topLeft = properties.ModelMatrix.TransformPoint(Math::Float3{ left, top, depth });
		const Math::Float3 topRight = properties.ModelMatrix.TransformPoint(Math::Float3{ left + width, top, depth });
		const Math::Float3 bottomRight = properties.ModelMatrix.TransformPoint(Math::Float3{ left + width, top + height, depth });
		const Math::Float3 bottomLeft = properties.ModelMatrix.TransformPoint(Math::Float3{ left, top + height, depth });

		// Create vertices for the rectangle
		submission.Vertices = {
			HybridRendererGenericVertex{ .Position = topLeft,		.Color = color },
			HybridRendererGenericVertex{ .Position = topRight,		.Color = color },
			HybridRendererGenericVertex{ .Position = bottomRight,	.Color = color },
			HybridRendererGenericVertex{ .Position = bottomLeft,	.Color = color },
		};

		// Define the indices for two triangles that make up the rectangle
		submission.Indices = { 0, 1, 2, 2, 3, 0 };

		// Submit the batch
		SubmitBatch(submission);
	}

	void HybridRenderer::RenderRoundedRectangle(const Math::FloatBoundary& boundary, const Math::BorderRadius& cornerRadii, const Math::Float4& fillColor, const Math::Float4& strokeColor, float strokeWeight, const RenderingProperties& properties)
	{
		// Create an SDF rounded rectangle batch submission
		HybridRendererBatch<HybridRendererSDFRoundedRectVertex> submission = {
			.BlendMode = properties.BlendMode,
			.ClipRect = properties.ClippingRect,
		};

		// Get the depth for this shape
		const float depth = m_DepthProvider.GetAndIncrement();

		// Transform the rectangle corners using the model matrix
		const auto [left, top, width, height] = boundary;
		const Math::Float3 topLeft = properties.ModelMatrix.TransformPoint(Math::Float3{ left, top, depth });
		const Math::Float3 topRight = properties.ModelMatrix.TransformPoint(Math::Float3{ left + width, top, depth });
		const Math::Float3 bottomRight = properties.ModelMatrix.TransformPoint(Math::Float3{ left + width, top + height, depth });
		const Math::Float3 bottomLeft = properties.ModelMatrix.TransformPoint(Math::Float3{ left, top + height, depth });

		// Create vertices for the rounded rectangle
		submission.Vertices = {
			HybridRendererSDFRoundedRectVertex{ .WorldPosition = topLeft,		.LocalPosition = { -1.0f, -1.0f },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .RectSize = { width, height }, .CornerRadii = cornerRadii.TopLeft, },
			HybridRendererSDFRoundedRectVertex{ .WorldPosition = topRight,		.LocalPosition = {  1.0f, -1.0f },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .RectSize = { width, height }, .CornerRadii = cornerRadii.TopRight },
			HybridRendererSDFRoundedRectVertex{ .WorldPosition = bottomRight,	.LocalPosition = {  1.0f,  1.0f },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .RectSize = { width, height }, .CornerRadii = cornerRadii.BottomRight },
			HybridRendererSDFRoundedRectVertex{ .WorldPosition = bottomLeft,	.LocalPosition = { -1.0f,  1.0f },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .RectSize = { width, height }, .CornerRadii = cornerRadii.BottomLeft },
		};

		// Define the indices for two triangles that make up the rounded rectangle
		submission.Indices = { 0, 1, 2, 2, 3, 0 };

		// Submit the batch
		SubmitBatch(submission);
	}

	void HybridRenderer::RenderEllipse(const Math::Float2& center, const Math::Radius& radius, const Math::Float4& fillColor, const Math::Float4& strokeColor, const float strokeWeight, const RenderingProperties& properties)
	{
		// Create an SDF circle batch submission
		HybridRendererBatch<HybridRendererSDFCircleVertex> submission = {
			.BlendMode = properties.BlendMode,
			.ClipRect = properties.ClippingRect,
		};

		// Get the depth for this shape
		const float depth = m_DepthProvider.GetAndIncrement();

		// Transform the center point using the model matrix
		const Math::Float3 transformedCenter	= properties.ModelMatrix.TransformPoint(Math::Float3{ center.X, center.Y, depth });
		const Math::Float2 transformedRadius	= properties.ModelMatrix.TransformVector(Math::Float2{ radius.X, radius.Y });
		const Math::Float2 localTopLeft			= { -transformedRadius.X, -transformedRadius.Y };
		const Math::Float2 localTopRight		= {  transformedRadius.X, -transformedRadius.Y };
		const Math::Float2 localBottomRight		= {  transformedRadius.X,  transformedRadius.Y };
		const Math::Float2 localBottomLeft		= { -transformedRadius.X,  transformedRadius.Y };

		// Create vertices for the circle quad
		submission.Vertices = {
			HybridRendererSDFCircleVertex{ .WorldPosition = transformedCenter + Math::Float3{ localTopLeft, 0.0f },		.LocalPosition = { -1.0f, -1.0f, },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .CircleSize = { radius.X * 2.0f, radius.Y * 2.0f }, },
			HybridRendererSDFCircleVertex{ .WorldPosition = transformedCenter + Math::Float3{ localTopRight, 0.0f },	.LocalPosition = {  1.0f, -1.0f, },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .CircleSize = { radius.X * 2.0f, radius.Y * 2.0f }, },
			HybridRendererSDFCircleVertex{ .WorldPosition = transformedCenter + Math::Float3{ localBottomRight, 0.0f },	.LocalPosition = {  1.0f,  1.0f, },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .CircleSize = { radius.X * 2.0f, radius.Y * 2.0f }, },
			HybridRendererSDFCircleVertex{ .WorldPosition = transformedCenter + Math::Float3{ localBottomLeft, 0.0f },	.LocalPosition = { -1.0f,  1.0f, },	.FillColor = fillColor, .StrokeColor = strokeColor, .StrokeWeight = strokeWeight, .CircleSize = { radius.X * 2.0f, radius.Y * 2.0f }, },
		};

		// Define the indices for two triangles that make up the quad
		submission.Indices = { 0, 1, 2, 2, 3, 0 };

		// Submit the batch
		SubmitBatch(submission);
	}

	void HybridRenderer::FillTriangle(const Math::Float2& p1, const Math::Float2& p2, const Math::Float2& p3, const Math::Float4& color, const RenderingProperties& properties)
	{
		// Create a generic batch submission for the triangle
		HybridRendererBatch<HybridRendererGenericVertex> submission = {
			.BlendMode = properties.BlendMode,
			.ClipRect = properties.ClippingRect,
		};

		// Get the depth for this shape
		const float depth = m_DepthProvider.GetAndIncrement();

		// Transform the triangle points using the model matrix
		const Math::Float3 tp1 = properties.ModelMatrix.TransformPoint(Math::Float3{ p1.X, p1.Y, depth });
		const Math::Float3 tp2 = properties.ModelMatrix.TransformPoint(Math::Float3{ p2.X, p2.Y, depth });
		const Math::Float3 tp3 = properties.ModelMatrix.TransformPoint(Math::Float3{ p3.X, p3.Y, depth });

		// Create vertices for the triangle
		submission.Vertices = {
			HybridRendererGenericVertex{ .Position = tp1, .Color = color },
			HybridRendererGenericVertex{ .Position = tp2, .Color = color },
			HybridRendererGenericVertex{ .Position = tp3, .Color = color },
		};

		// Define the indices for the triangle
		submission.Indices = { 0, 1, 2 };

		// Submit the batch
		SubmitBatch(submission);
	}

	HybridRenderer::HybridRenderer(
		const uint32_t genericVertexArrayId,
		const uint32_t genericVertexBufferId,
		const uint32_t genericElementBufferId,
		const size_t genericVertexBufferCapacity,
		const size_t genericElementBufferCapacity,
		std::unique_ptr<Shader> genericShader,
		const uint32_t sdfCircleVertexArrayId,
		const uint32_t sdfCircleVertexBufferId,
		const uint32_t sdfCircleElementBufferId,
		const size_t sdfCircleVertexBufferCapacity,
		const size_t sdfCircleElementBufferCapacity,
		std::unique_ptr<Shader> sdfCircleShader,
		const uint32_t sdfRoundedRectVertexArrayId,
		const uint32_t sdfRoundedRectVertexBufferId,
		const uint32_t sdfRoundedRectElementBufferId,
		const size_t sdfRoundedRectVertexBufferCapacity,
		const size_t sdfRoundedRectElementBufferCapacity,
		std::unique_ptr<Shader> sdfRoundedRectShader
	) : m_GenericVertexArrayId(genericVertexArrayId),
		m_GenericVertexBufferId(genericVertexBufferId),
		m_GenericElementBufferId(genericElementBufferId),
		m_GenericVertexBufferCapacity(genericVertexBufferCapacity),
		m_GenericElementBufferCapacity(genericElementBufferCapacity),
		m_GenericShader(std::move(genericShader)),
		m_SDFCircleVertexArrayId(sdfCircleVertexArrayId),
		m_SDFCircleVertexBufferId(sdfCircleVertexBufferId),
		m_SDFCircleElementBufferId(sdfCircleElementBufferId),
		m_SDFCircleVertexBufferCapacity(sdfCircleVertexBufferCapacity),
		m_SDFCircleElementBufferCapacity(sdfCircleElementBufferCapacity),
		m_SDFCircleShader(std::move(sdfCircleShader)),
		m_SDFRoundedRectVertexArrayId(sdfRoundedRectVertexArrayId),
		m_SDFRoundedRectVertexBufferId(sdfRoundedRectVertexBufferId),
		m_SDFRoundedRectElementBufferId(sdfRoundedRectElementBufferId),
		m_SDFRoundedRectVertexBufferCapacity(sdfRoundedRectVertexBufferCapacity),
		m_SDFRoundedRectElementBufferCapacity(sdfRoundedRectElementBufferCapacity),
		m_SDFRoundedRectShader(std::move(sdfRoundedRectShader))
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

		glBindVertexArray(m_GenericVertexArrayId);
		glUseProgram(m_GenericShader->GetShaderId());
		m_GenericShader->UploadFloatMatrix4x4("u_ProjectionMatrix", m_ProjectionMatrix.GetData());

		static const auto activateBlendMode = GetBlendModeActivator();
		static const auto activateClipRect = GetClipRectActivator();

		auto start = std::chrono::high_resolution_clock::now();
		int iterations = 0;
		Chunked<HybridRendererGenericVertex>(
			m_GenericBatches,
			m_GenericVertexBufferCapacity,
			m_GenericElementBufferCapacity,
			[this, &iterations](ReadOnlyChunk<HybridRendererGenericVertex>&& chunk)
			{
				Warning(std::format("HybridRenderer: Flushing generic chunk with {} vertices and {} indices.", chunk.Vertices.size(), chunk.Indices.size()));

				// Maybe we can do this once at the start?
				activateBlendMode(*chunk.BlendMode);
				activateClipRect(*chunk.ClipRect);

				// Upload the data to the GPU
				glNamedBufferSubData(m_GenericVertexBufferId, 0, chunk.Vertices.size() * sizeof(HybridRendererGenericVertex), chunk.Vertices.data());
				glNamedBufferSubData(m_GenericElementBufferId, 0, chunk.Indices.size() * sizeof(uint32_t), chunk.Indices.data());

				// Draw the chunk
				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(chunk.Indices.size()), GL_UNSIGNED_INT, nullptr); //!< Maybe get offset pointer correct?
				++iterations;
			}
		);
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> duration = end - start;
		Info(std::format("HybridRenderer: Flushed generic chunks in {:.2f} ms.", duration.count()));
		Info(std::format("HybridRenderer: Flushed {} generic chunks in {} iterations.", m_GenericBatches.size(), iterations));

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

		static const auto blendModeActivator = GetBlendModeActivator();
		static const auto clipRectActivator = GetClipRectActivator();

		// Unfortunately, we can not assume that the batches are small enough to fit into the
		// vertex buffers / element buffers. In order to submit all data correctly, we need to
		// split batches that exceed the buffer capacities into smaller sub-batches (chunks).
		glBindVertexArray(m_SDFCircleVertexArrayId);
		glUseProgram(m_SDFCircleShader->GetShaderId());
		m_SDFCircleShader->UploadFloatMatrix4x4("u_ProjectionMatrix", m_ProjectionMatrix.GetData());

		Chunked(
			m_SDFCircleBatches,
			m_SDFCircleVertexBufferCapacity,
			m_SDFCircleElementBufferCapacity,
			[this](ReadOnlyChunk<HybridRendererSDFCircleVertex>&& chunk)
			{
				// Maybe we can do this once at the start?
				blendModeActivator(*chunk.BlendMode);
				clipRectActivator(*chunk.ClipRect);

				// Upload the data to the GPU
				glNamedBufferSubData(m_SDFCircleVertexBufferId, 0, chunk.Vertices.size() * sizeof(HybridRendererSDFCircleVertex), chunk.Vertices.data());
				glNamedBufferSubData(m_SDFCircleElementBufferId, 0, chunk.Indices.size() * sizeof(uint32_t), chunk.Indices.data());

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

		static const auto blendModeActivator = GetBlendModeActivator();
		static const auto clipRectActivator = GetClipRectActivator();

		// Unfortunately, we can not assume that the batches are small enough to fit into the
		// vertex buffers / element buffers. In order to submit all data correctly, we need to
		// split batches that exceed the buffer capacities into smaller sub-batches (chunks).

		glBindVertexArray(m_SDFRoundedRectVertexArrayId);
		glUseProgram(m_SDFRoundedRectShader->GetShaderId());
		m_SDFRoundedRectShader->UploadFloatMatrix4x4("u_ProjectionMatrix", m_ProjectionMatrix.GetData());

		Chunked(
			m_SDFRoundedRectBatches,
			m_SDFRoundedRectVertexBufferCapacity,
			m_SDFRoundedRectElementBufferCapacity,
			[this](ReadOnlyChunk<HybridRendererSDFRoundedRectVertex>&& chunk)
			{
				// Maybe we can do this once at the start?
				blendModeActivator(*chunk.BlendMode);
				clipRectActivator(*chunk.ClipRect);

				// Upload the data to the GPU
				glNamedBufferSubData(m_SDFRoundedRectVertexBufferId, 0, chunk.Vertices.size() * sizeof(HybridRendererSDFRoundedRectVertex), chunk.Vertices.data());
				glNamedBufferSubData(m_SDFRoundedRectElementBufferId, 0, chunk.Indices.size() * sizeof(uint32_t), chunk.Indices.data());

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

	void HybridRenderer::SubmitBatch(const HybridRendererBatch<HybridRendererSDFCircleVertex>& submission)
	{
		MergeOrCreateBatch(m_SDFCircleBatches, submission);
	}

	void HybridRenderer::SubmitBatch(const HybridRendererBatch<HybridRendererSDFRoundedRectVertex>& submission)
	{
		MergeOrCreateBatch(m_SDFRoundedRectBatches, submission);
	}
}
