// Project Name : DirectGL
// File Name    : DirectGL-HybridRenderer.ixx
// Author       : Felix Busch
// Created Date : 2025/11/18

module;

#include <memory>
#include <cstdint>
#include <vector>
#include <map>

export module DirectGL:HybridRenderer;

import DirectGL.Math;

import :RenderingProperties;
import :DepthProvider;
import :Shader;

namespace DGL
{

	/// @brief This structure represents a vertex used for generic rendering in the HybridRenderer.
	///
	/// All shapes using non-sdf rendering in the HybridRenderer will use this format.
	/// These shapes include:
	///		- Outlines of all shapes
	///		- Rectangles (non-rounded)
	///		- Triangles
	///		- Lines (including stroke caps)
	///		- Custom shapes (Polygons, Beziers, etc.)
	struct HybridRendererGenericVertex
	{
		Math::Float3 Position;	//!< The position of the vertex in 3D space.
		Math::Float4 Color;		//!< The color of the vertex.
	};

	/// @brief This structure represents a vertex used for textured rendering in the HybridRenderer.
	///
	/// All shapes using textured rendering in the HybridRenderer will use this format.
	/// These shapes include:
	/// 	- Images
	///		- Text
	//struct HybridRendererTexturedVertex
	//{
	//	Math::Float3 Position;	//!< The position of the vertex in 3D space.
	//	Math::Float2 TexCoord;	//!< The texture coordinates of the vertex.
	//	Math::Float4 Color;		//!< The color of the vertex.
	//};

	/// @brief This structure represents a vertex used for SDF circle rendering in the HybridRenderer.
	///
	/// The reason for having a dedicated structure for SDF circle rendering
	/// is that SDF rendering requires additional data for accurate
	/// calculations, such as local position within the shape.
	///
	/// Rounded-Rectangles are excluded from this structure as they require
	/// a more complex set of data due to their corners and varying radii.
	///
	/// All round shapes using SDF rendering in the HybridRenderer will use this format.
	/// These shapes include:
	///		- Circles
	///		- Ellipsis
	struct HybridRendererSDFCircleVertex
	{
		Math::Float3	WorldPosition;	//!< The position of the vertex in world space.
		Math::Float2	LocalPosition;	//!< The local position used for SDF calculations.
		Math::Float4	FillColor;		//!< The fill color of the shape.
		Math::Float4	StrokeColor;	//!< The stroke color of the shape.
		float			StrokeWeight;	//!< The stroke weight of the shape.
	};

	/// @brief This structure represents a vertex used for SDF rounded-rectangle rendering in the HybridRenderer.
	///
	/// The reason for having a dedicated structure for SDF rounded-rectangle rendering
	/// is that SDF rendering requires additional data for accurate
	/// calculations, such as local position within the shape and border radii.
	///
	/// All rounded-rectangle shapes using SDF rendering in the HybridRenderer will use this format.
	/// These shapes include:
	/// 		- Rounded Rectangles
	struct HybridRendererSDFRoundedRectVertex
	{
		Math::Float3	WorldPosition;	//!< The position of the vertex in world space.
		Math::Float2	LocalPosition;	//!< The local position used for SDF calculations.
		Math::Float4	FillColor;		//!< The fill color of the shape.
		Math::Float4	StrokeColor;	//!< The stroke color of the shape.
		float			StrokeWeight;	//!< The stroke weight of the shape.
		Math::Float4	BorderRadii;	//!< The border radii for each corner (top-left, top-right, bottom-right, bottom-left).
	};

	/// @brief This structure represents a batch of generic shape data to be rendered.
	///
	/// This structure may hold the following types of vertices:
	/// 	- HybridRendererGenericVertex
	/// 	- HybridRendererTexturedVertex
	/// 	- HybridRendererSDFRoundedRectVertex
	/// 	- HybridRendererSDFCircleVertex
	///
	/// The batch contains all necessary information to render a set of shapes
	/// using the same rendering properties, such as blend mode and clipping rectangle.
	///
	/// We use this structure to group shapes together for efficient rendering.
	/// This also allows us to minimize state changes in the graphics pipeline.
	///
	/// In order to group them we need a way to compare the rendering properties.
	/// This is done by calculating a hash of the properties and using that
	/// as a key in a map to group the shapes together.
	template <typename Vertex>
	struct HybridRendererBatch
	{
		BlendMode				BlendMode;	//!< The blend mode to use for this batch.
		ClipRect				ClipRect;	//!< The clipping rectangle to use for this batch.
		std::vector<Vertex>		Vertices;	//!< The SDF shape data to render.
		std::vector<uint32_t>	Indices;	//!< The indices defining the order of vertices for rendering.
	};

	/// @brief Type alias for the key used to identify batches in the HybridRenderer.
	using HybridRendererBatchKey = size_t;

	/// @brief This renderer combines Geometry-based rendering with SDF-based rendering.
	///
	/// Each shape that is per definition "round", are rendered using SDF techniques, while
	/// all other shapes are tessellated into geometry and rendered using traditional methods.
	///
	/// The following shapes are rendered using SDF techniques:
	///		* Circles
	///		* Ellipsis
	///		* Rounded Rectangles
	///
	///	The following shapes are rendered using traditional geometry-based rendering:
	///		* Lines (including stroke caps)
	///		* Triangles
	///		* Rectangles
	///		* Images
	///		* Text
	///		* Custom shapes (Polygons, Beziers, etc.)
	///
	/// Outlines also differ in their rendering approach:
	///		- Outlines of round shapes are rendered using SDF techniques.
	///		- Outlines of non-round shapes are rendered using traditional geometry-based methods.
	///
	///	Geometry-based outlines are created by generating additional geometry that follows the
	///	contour of the shape, taking into account the stroke weight and join/cap styles.
	class HybridRenderer
	{
	public:

		/// @brief Create a new instance of a HybridRenderer.
		///
		/// This function creates a new instance of a HybridRenderer,
		/// initializing all necessary resources for rendering.
		/// This includes setting up shaders, buffers, and any other
		/// state required for hybrid rendering. Therefor an OpenGL
		/// context must be active when calling this function.
		/// 
		/// @return A unique pointer to the created HybridRenderer instance.
		static std::unique_ptr<HybridRenderer> Create();

		/// @brief Cleans up all resources used by the HybridRenderer.
		///
		/// This destructor ensures that all resources allocated
		/// by the HybridRenderer are properly released.
		~HybridRenderer();

		/// @brief This function sets the viewport for the renderer.
		///
		/// The viewport is key in order to compute the correct
		/// projection matrix for rendering. This renderer will
		/// use an orthographic projection matrix based on the
		/// provided viewport.
		/// 
		/// @param viewport The viewport to set for the renderer.
		void SetViewport(const Math::FloatBoundary& viewport);

		void BeginDraw();
		void EndDraw();

		/// @brief Fill a rectangle with the specified color and rendering properties.
		///
		/// This function fills a rectangle defined by the given boundary
		/// with the specified color, using the provided rendering properties.
		///
		/// The rectangle is being rendered using traditional geometry-based methods.
		/// This involves tessellating the rectangle into two triangles
		/// and submitting them to the graphics pipeline for rendering.
		/// 
		/// @param boundary The boundary of the rectangle to fill.
		/// @param color The color to fill the rectangle with.
		/// @param properties The rendering properties to use for this operation.
		void FillRectangle(const Math::FloatBoundary& boundary, const Math::Float4& color, const RenderingProperties& properties);

		/// @brief Fill a triangle with the specified color and rendering properties.
		///
		/// This function fills a triangle defined by the three given points
		/// with the specified color, using the provided rendering properties.
		///
		/// The triangle is being rendered using traditional geometry-based methods.
		/// This involves submitting the triangle vertices to the graphics pipeline for rendering.
		///
		/// @param p1 The first point of the triangle.
		/// @param p2 The second point of the triangle.
		/// @param p3 The third point of the triangle.
		/// @param color The color to fill the triangle with.
		/// @param properties The rendering properties to use for this operation.
		void FillTriangle(const Math::Float2& p1, const Math::Float2& p2, const Math::Float2& p3, const Math::Float4& color, const RenderingProperties& properties);

	private:

		explicit HybridRenderer(
			uint32_t genericVertexArrayId,
			uint32_t genericVertexBufferId,
			uint32_t genericElementBufferId,
			size_t genericVertexBufferCapacity,
			size_t genericElementBufferCapacity,
			std::unique_ptr<Shader> genericShader
		);

		void Flush();

		void FlushGenericBatch();
		void FlushSDFCircleBatch();
		void FlushSDFRoundedRectBatch();

		/// Submit a batch of shapes to be rendered.
		///
		/// This function computes the index where the submission
		/// should be appended to the internal batch list and
		/// appends the submission accordingly.
		///
		/// This is done to group shapes with similar rendering
		/// properties together, minimizing state changes in the
		/// graphics pipeline and improving rendering performance.
		
		void SubmitBatch(const HybridRendererBatch<HybridRendererGenericVertex>& submission);
		void SubmitBatch(const HybridRendererBatch<HybridRendererSDFCircleVertex>& submission);
		void SubmitBatch(const HybridRendererBatch<HybridRendererSDFRoundedRectVertex>& submission);

	private:

		// In this case we use std::map as we need the batches to be in a specific order
		// for efficient rendering (minimizing state changes).
		std::map<HybridRendererBatchKey, HybridRendererBatch<HybridRendererGenericVertex>>			m_GenericBatches;
		std::map<HybridRendererBatchKey, HybridRendererBatch<HybridRendererSDFCircleVertex>>		m_SDFCircleBatches;
		std::map<HybridRendererBatchKey, HybridRendererBatch<HybridRendererSDFRoundedRectVertex>>	m_SDFRoundedRectBatches;

		uint32_t	m_GenericVertexArrayId = 0;
		uint32_t	m_GenericVertexBufferId = 0;
		uint32_t	m_GenericElementBufferId = 0;
		size_t		m_GenericVertexBufferCapacity = 0;
		size_t		m_GenericElementBufferCapacity = 0;

		std::unique_ptr<Shader> m_GenericShader;

		DepthProvider m_DepthProvider;

		Math::Matrix4x4 m_ProjectionMatrix;

	};
}