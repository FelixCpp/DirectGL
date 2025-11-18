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

		return std::unique_ptr<HybridRenderer>(
			new HybridRenderer(
				genericVertexArrayId,
				genericVertexBufferId,
				genericElementBufferId,
				GenericVertexBufferCapacity,
				GenericElementBufferCapacity,
				std::move(genericShader)
			)
		);
	}

	HybridRenderer::~HybridRenderer()
	{
	}

	void HybridRenderer::SetViewport(const Math::FloatBoundary& viewport)
	{
		m_ProjectionMatrix = Math::Matrix4x4::Orthographic(viewport, -1.0f, 1.0f);
	}

	void HybridRenderer::BeginDraw()
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
		std::unique_ptr<Shader> genericShader
	) : m_GenericVertexArrayId(genericVertexArrayId),
		m_GenericVertexBufferId(genericVertexBufferId),
		m_GenericElementBufferId(genericElementBufferId),
		m_GenericVertexBufferCapacity(genericVertexBufferCapacity),
		m_GenericElementBufferCapacity(genericElementBufferCapacity),
		m_GenericShader(std::move(genericShader))
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
