// Project Name : DirectGL
// File Name    : DirectGL-HybridRenderer.ixx
// Author       : Felix Busch
// Created Date : 2025/11/18

module;

#include <memory>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <span>

export module DirectGL:HybridRenderer;

import DirectGL.Math;

import :RenderingProperties;
import :DepthProvider;
import :Shader;
import :Stroker;

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
		Math::Float2	CircleSize;		//!< The size of the circle (radiusX, radiusY).
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
		Math::Float2	RectSize;		//!< The size of the rectangle (width, height).
		float			CornerRadius;	//!< The border radii for each corner (top-left, top-right, bottom-right, bottom-left).
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
		using VertexType = Vertex;

		BlendMode				BlendMode;	//!< The blend mode to use for this batch.
		ClipRect				ClipRect;	//!< The clipping rectangle to use for this batch.
		std::vector<Vertex>		Vertices;	//!< The SDF shape data to render.
		std::vector<uint32_t>	Indices;	//!< The indices defining the order of vertices for rendering.
	};

	template <typename Vertex>
	struct HybridSDFRendererBatch
	{
		using VertexType = Vertex;

		BlendMode				BlendMode;			//!< The blend mode to use for this batch.
		ClipRect				ClipRect;			//!< The clipping rectangle to use for this batch.
		std::vector<Vertex>		Vertices;			//!< The SDF shape data to render.
		std::vector<uint32_t>	Indices;			//!< The indices defining the order of vertices for rendering.
		bool					IsFillEnabled;		//!< Whether to fill a border or not
		bool					IsStrokeEnabled;	//!< Whether to apply a border or not
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

		/// @brief Begin a new drawing session.
		///
		/// This function prepares the renderer for a new drawing session.
		/// It clears any previous state and sets up necessary
		/// configurations for rendering.
		/// 
		/// @note An OpenGL context must be active when calling this function.
		void BeginDraw();

		/// @brief End the current drawing session.
		///
		/// This function finalizes the current drawing session.
		/// It flushes any pending draw calls and resets
		/// the renderer state.
		///
		/// @note An OpenGL context must be active when calling this function.
		void EndDraw();

		/// @brief Renders a rectangle with the specified parameters.
		/// @param boundary The boundary of the rectangle.
		/// @param fillColor The fill color of the rectangle.
		/// @param strokeColor The stroke color of the rectangle.
		/// @param strokeWeight The weight of the stroke around the rectangle.
		/// @param strokeProperties The stroke properties to use for this rectangle.
		/// @param properties Additional rendering properties to use for this operation.
		void RenderRectangle(const Math::FloatBoundary& boundary, const Math::Float4& fillColor, const Math::Float4& strokeColor, float strokeWeight, const StrokeProperties& strokeProperties, const RenderingProperties& properties);

		/// @brief Renders a rounded rectangle with the specified parameters.
		/// @param boundary The boundary of the rounded rectangle.
		/// @param cornerRadii The corner radii for each corner of the rounded rectangle.
		/// @param fillColor The fill color of the rounded rectangle.
		/// @param strokeColor The stroke color of the rounded rectangle.
		/// @param strokeWeight The weight of the stroke around the rounded rectangle.
		/// @param properties Additional rendering properties to use for this operation.
		void RenderRoundedRectangle(const Math::FloatBoundary& boundary, const Math::BorderRadius& cornerRadii, const Math::Float4& fillColor, const Math::Float4& strokeColor, float strokeWeight, const RenderingProperties& properties);

		/// @brief Render an ellipse with the specified parameters.
		///
		/// This function renders an ellipse defined by its center and radius,
		/// with the specified fill and stroke colors, stroke weight,
		/// and rendering properties.
		///
		/// The ellipse is rendered using Signed Distance Field (SDF) techniques.
		/// This allows for smooth edges and scalable rendering without loss of quality.
		/// 
		/// @param center The center point of the ellipse.
		/// @param radius The radius of the ellipse.
		/// @param fillColor The fill color of the ellipse.
		/// @param strokeColor The stroke color of the ellipse.
		/// @param strokeWeight The weight of the stroke around the ellipse.
		/// @param properties The rendering properties to use for this operation.
		void RenderEllipse(const Math::Float2& center, const Math::Radius& radius, const Math::Float4& fillColor, const Math::Float4& strokeColor, float strokeWeight, const RenderingProperties& properties);

	private:

		/// @brief Properties related to generic geometry-based rendering.
		///
		/// This structure holds all necessary properties
		/// for rendering shapes using traditional geometry-based methods.
		/// This includes vertex array objects, buffers, and shaders.
		/// These properties are used when rendering shapes
		/// that do not require SDF techniques.
		///
		/// Shapes using these properties include:
		/// 	- Rectangles (non-rounded)
		/// 	- Triangles
		/// 	- Lines (including stroke caps)
		/// 	- Custom shapes (Polygons, Beziers, etc.)
		/// 	- Outlines of any shape
		struct GenericRenderingProperties
		{
			uint32_t				VertexArrayId;
			uint32_t				VertexBufferId;
			uint32_t				ElementBufferId;
			size_t					VertexBufferCapacity;
			size_t					ElementBufferCapacity;
			std::unique_ptr<Shader>	Shader;
		};

		/// @brief Properties related to SDF circle rendering.
		///
		/// This structure holds all necessary properties
		/// for rendering shapes using Signed Distance Field (SDF) techniques
		/// specifically for circles and ellipses.
		/// This includes vertex array objects, buffers, and shaders.
		/// These properties are used when rendering round shapes
		/// that benefit from SDF rendering.
		///
		/// Shapes using these properties include:
		/// 	- Circles	(+ stroke)
		///		- Ellipsis	(+ stroke)
		struct SDFCircleRenderingProperties
		{
			uint32_t				VertexArrayId;
			uint32_t				VertexBufferId;
			uint32_t				ElementBufferId;
			size_t					VertexBufferCapacity;
			size_t					ElementBufferCapacity;
			std::unique_ptr<Shader>	Shader;
		};

		/// @brief Properties related to SDF rounded-rectangle rendering.
		///
		/// This structure holds all necessary properties
		/// for rendering shapes using Signed Distance Field (SDF) techniques
		/// specifically for rounded rectangles.
		/// This includes vertex array objects, buffers, and shaders.
		/// These properties are used when rendering rounded-rectangle shapes
		/// that benefit from SDF rendering.
		///
		/// Shapes using these properties include:
		/// 	- Rounded Rectangles (+ stroke)
		struct SDFRoundedRectRenderingProperties
		{
			uint32_t				VertexArrayId;
			uint32_t				VertexBufferId;
			uint32_t				ElementBufferId;
			size_t					VertexBufferCapacity;
			size_t					ElementBufferCapacity;
			std::unique_ptr<Shader>	Shader;
		};

	private:

		/// @brief Construct generic rendering properties.
		///
		/// This function constructs and initializes the
		/// generic rendering properties required for
		/// geometry-based rendering.
		/// 
		/// @return The constructed GenericRenderingProperties.
		static GenericRenderingProperties CreateGenericRenderingProperties();

		/// @brief Construct SDF circle rendering properties.
		///
		/// This function constructs and initializes the
		/// SDF circle rendering properties required for
		/// rendering circles and ellipses using SDF techniques.
		/// 
		/// @return The constructed SDFCircleRenderingProperties.
		static SDFCircleRenderingProperties CreateSDFCircleRenderingProperties();

		/// @brief Construct SDF rounded-rectangle rendering properties.
		///
		/// This function constructs and initializes the
		/// SDF rounded-rectangle rendering properties required for
		/// rendering rounded rectangles using SDF techniques.
		///
		/// @return The constructed SDFRoundedRectRenderingProperties.
		static SDFRoundedRectRenderingProperties CreateSDFRoundedRectRenderingProperties();

	private:

		explicit HybridRenderer(
			GenericRenderingProperties&& genericProperties,
			SDFCircleRenderingProperties&& sdfCircleProperties,
			SDFRoundedRectRenderingProperties&& sdfRoundedRectProperties
		);

		/// Flush all pending batches to the GPU for rendering.
		/// These function iterates over all stored batches of shapes
		/// and submits them to the GPU for rendering.

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
		void SubmitBatch(const HybridSDFRendererBatch<HybridRendererSDFCircleVertex>& submission);
		void SubmitBatch(const HybridSDFRendererBatch<HybridRendererSDFRoundedRectVertex>& submission);

	private:

		// In this case we use std::map as we need the batches to be in a specific order
		// for efficient rendering (minimizing state changes).
		std::unordered_map<HybridRendererBatchKey, HybridRendererBatch<HybridRendererGenericVertex>>			m_GenericBatches;
		std::unordered_map<HybridRendererBatchKey, HybridSDFRendererBatch<HybridRendererSDFCircleVertex>>		m_SDFCircleBatches;
		std::unordered_map<HybridRendererBatchKey, HybridSDFRendererBatch<HybridRendererSDFRoundedRectVertex>>	m_SDFRoundedRectBatches;

		GenericRenderingProperties			m_GenericProperties;
		SDFCircleRenderingProperties		m_SDFCircleProperties;
		SDFRoundedRectRenderingProperties	m_SDFRoundedRectProperties;

		DepthProvider m_DepthProvider;

		Math::Matrix4x4		m_ProjectionMatrix;
		Math::FloatBoundary	m_Viewport;

		PolygonStroker m_Stroker;

	};
}