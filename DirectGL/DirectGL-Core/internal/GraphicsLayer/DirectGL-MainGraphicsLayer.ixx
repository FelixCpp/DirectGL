// Project Name : DirectGL
// File Name    : DirectGL-MainGraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/11/05

module;

#include <memory>

export module DirectGL:MainGraphicsLayer;

import :GraphicsLayer;
import :RenderStyleStack;
import :MeshRenderer;
import :DepthProvider;

namespace DGL
{
	class MainGraphicsLayer : public GraphicsLayer
	{
	public:

		explicit MainGraphicsLayer(
			const std::weak_ptr<MeshRenderer>& meshRenderer,
			const Math::FloatBoundary& viewport
		);

		void SetViewport(const Math::FloatBoundary& viewport);
		const Math::FloatBoundary& GetViewport() const;

		void BeginDraw();
		void EndDraw();

		void PushStyle(bool extendCurrentStyle) override;
		void PopStyle() override;
		RenderStyle& PeekStyle() override;

		void SetRectMode(const RectMode& mode) override;
		void SetEllipseMode(const EllipseMode& mode) override;
		void SetEllipseSegmentsMode(const EllipseSegmentsMode& mode) override;

		void SetFillColor(color_t color) override;
		void SetFillColorDisabled() override;

		void SetStrokeColor(color_t color) override;
		void SetStrokeColorDisabled() override;
		void SetStrokeWeight(float weight) override;
		void SetStrokeJoin(StrokeJoin joinStyle) override;
		void SetStrokeCap(StrokeCap strokeCap) override;

		void BeginShape(ShapeMode mode) override;
		void EndShape(ShapeClosingMode mode) override;
		void Vertex(float x, float y) override;

		void Background(color_t color) override;
		void Rect(float x1, float y1, float x2, float y2) override;
		void Ellipse(float x1, float y1, float x2, float y2) override;
		void Point(float x, float y) override;
		void Line(float x1, float y1, float x2, float y2) override;

	private:

		void Render(const Mesh& mesh);
		float GetCurrentDepth();

		RenderStyleStack m_RenderStyleStack;
		std::weak_ptr<MeshRenderer> m_MeshRenderer;
		DepthProvider m_DepthProvider;

		Math::FloatBoundary m_Viewport;
		Math::Matrix4x4 m_ProjectionMatrix;

	};
}