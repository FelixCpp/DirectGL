module;

#include <array>
#include <optional>
#include <string_view>

module DirectGL;

import :MainGraphicsLayer;

namespace DGL
{
	[[nodiscard]] RenderingProperties StyleToRenderingProperties(const RenderStyle& style)
	{
		return RenderingProperties {
			.ClippingRect		= style.ClipRect,
			.BlendMode			= style.BlendMode,
			.ModelMatrix		= style.MatrixStack.PeekMatrix(),
			.Shader				= style.Shader,
			.IsFillEnabled		= style.IsFillEnabled,
			.IsStrokeEnabled	= style.IsStrokeEnabled,
		};
	}

	[[nodiscard]] StrokeProperties StyleToStrokeProperties(const RenderStyle& style)
	{
		return StrokeProperties {
			.MiterLimit		= 10.0f,
			.Alignment		= style.StrokeAlignment,
			.StrokeJoin		= style.JoinStyle,
			.StrokeCap		= StrokeCap::Round,
			.IsClosed		= false,
		};
	}

	MainGraphicsLayer::MainGraphicsLayer(const std::weak_ptr<Renderer>& renderer, const Math::FloatBoundary& viewport):
		m_Renderer(renderer),
		m_RenderTarget(viewport)
	{
	}

	void MainGraphicsLayer::SetViewport(const Math::FloatBoundary& viewport)
	{
		m_RenderTarget.SetViewport(viewport);
		m_Renderer.lock()->SetViewport(viewport);
	}

	const Math::FloatBoundary& MainGraphicsLayer::GetViewport() const
	{
		return m_RenderTarget.GetViewport();
	}

	void MainGraphicsLayer::BeginDraw()
	{
		m_RenderStyleStack.Reset();
		m_Renderer.lock()->BeginDraw();
		m_RenderTarget.Activate();
	}

	void MainGraphicsLayer::EndDraw()
	{
		m_Renderer.lock()->EndDraw();
	}

	void MainGraphicsLayer::PushStyle(const bool extendCurrentStyle)
	{
		m_RenderStyleStack.PushStyle(extendCurrentStyle ? PeekStyle() : RenderStyle());
	}

	void MainGraphicsLayer::PopStyle()
	{
		m_RenderStyleStack.PopStyle();
	}

	RenderStyle& MainGraphicsLayer::PeekStyle()
	{
		return m_RenderStyleStack.PeekStyle();
	}

	void MainGraphicsLayer::PushMatrix(const bool extendCurrentMatrix)
	{
		PeekStyle().MatrixStack.PushMatrix(extendCurrentMatrix);
	}

	void MainGraphicsLayer::PopMatrix()
	{
		PeekStyle().MatrixStack.PopMatrix();
	}

	Math::Matrix4x4& MainGraphicsLayer::PeekMatrix()
	{
		return PeekStyle().MatrixStack.PeekMatrix();
	}

	void MainGraphicsLayer::Translate(const float x, const float y)
	{
		Math::Matrix4x4& matrix = PeekMatrix();
		matrix = matrix * Math::Matrix4x4::Translation(x, y, 0.0f);
	}

	void MainGraphicsLayer::Rotate(const Math::Angle angle)
	{
		Math::Matrix4x4& matrix = PeekMatrix();
		matrix = matrix * Math::Matrix4x4::Rotation(angle);
	}

	void MainGraphicsLayer::Scale(const float scaleX, const float scaleY)
	{
		Math::Matrix4x4& matrix = PeekMatrix();
		matrix = matrix * Math::Matrix4x4::Scaling(scaleX, scaleY, 1.0f);
	}

	void MainGraphicsLayer::Shear(const Math::Angle shearX, const Math::Angle shearY)
	{
		Math::Matrix4x4& matrix = PeekMatrix();
		matrix = matrix * Math::Matrix4x4::Skew(shearX, shearY);
	}

	void MainGraphicsLayer::SetRectMode(const RectMode& mode)
	{
		RenderStyle& style = PeekStyle();
		style.RectMode = mode;
	}

	void MainGraphicsLayer::SetEllipseMode(const EllipseMode& mode)
	{
		RenderStyle& style = PeekStyle();
		style.EllipseMode = mode;
	}

	void MainGraphicsLayer::SetEllipseSegmentsMode(const EllipseSegmentsMode& mode)
	{
		RenderStyle& style = PeekStyle();
		style.EllipseSegmentsMode = mode;
	}

	void MainGraphicsLayer::SetFillColor(const color_t color)
	{
		RenderStyle& style = PeekStyle();
		style.FillColor = color;
		style.IsFillEnabled = true;
	}

	void MainGraphicsLayer::SetFillColorDisabled()
	{
		RenderStyle& style = PeekStyle();
		style.IsFillEnabled = false;
	}

	void MainGraphicsLayer::SetStrokeColor(const color_t color)
	{
		RenderStyle& style = PeekStyle();
		style.StrokeColor = color;
		style.IsStrokeEnabled = true;
	}

	void MainGraphicsLayer::SetStrokeColorDisabled()
	{
		RenderStyle& style = PeekStyle();
		style.IsStrokeEnabled = false;
	}

	void MainGraphicsLayer::SetStrokeWeight(const float weight)
	{
		RenderStyle& style = PeekStyle();
		style.StrokeWeight = weight;
	}

	void MainGraphicsLayer::SetStrokeJoin(const StrokeJoin joinStyle)
	{
		RenderStyle& style = PeekStyle();
		style.JoinStyle = joinStyle;
	}

	void MainGraphicsLayer::SetStrokeCap(const StrokeCap strokeCap)
	{
		RenderStyle& style = PeekStyle();
		style.StrokeCap = strokeCap;
	}

	void MainGraphicsLayer::SetStrokeAlignment(const StrokeAlignment alignment)
	{
		RenderStyle& style = PeekStyle();
		style.StrokeAlignment = alignment;
	}

	void MainGraphicsLayer::SetBlendMode(const BlendMode& mode)
	{
		RenderStyle& style = PeekStyle();
		style.BlendMode = mode;
	}

	void MainGraphicsLayer::SetClipRectMode(const RectMode& mode)
	{
		RenderStyle& style = PeekStyle();
		style.ClipRectMode = mode;
	}

	void MainGraphicsLayer::SetClipRect(const float x1, const float y1, const float x2, const float y2)
	{
		RenderStyle& style = PeekStyle();
		style.ClipRect = ClipRect::Clipped(Math::IntBoundary::FromLTWH(static_cast<int>(x1), static_cast<int>(y1), static_cast<int>(x2), static_cast<int>(y2)));
	}

	void MainGraphicsLayer::SetClipRectDisabled()
	{
		RenderStyle& style = PeekStyle();
		style.ClipRect = ClipRect::Unclipped();
	}

	void MainGraphicsLayer::SetTextSize(const float size)
	{
		RenderStyle& style = PeekStyle();
		style.TextSize = size;
	}

	void MainGraphicsLayer::SetTextFont(Font* font)
	{
		RenderStyle& style = PeekStyle();
		style.Font = font;
	}

	void MainGraphicsLayer::SetTextAlign(const TextAlignment alignment)
	{
		RenderStyle& style = PeekStyle();
		style.TextAlign = alignment;
	}

	void MainGraphicsLayer::SetImageTint(const color_t tint)
	{
		RenderStyle& style = PeekStyle();
		style.ImageTint = tint;
	}

	void MainGraphicsLayer::SetImageMode(const RectMode& mode)
	{
		RenderStyle& style = PeekStyle();
		style.ImageMode = mode;
	}

	void MainGraphicsLayer::SetImageSampler(const ImageSampler* sampler)
	{
		RenderStyle& style = PeekStyle();
		style.ImageSampler = sampler;
	}

	void MainGraphicsLayer::SetShader(Shader* shader)
	{
		RenderStyle& style = PeekStyle();
		style.Shader = shader;
	}

	void MainGraphicsLayer::BeginShape(const ShapeMode mode)
	{
	}

	void MainGraphicsLayer::EndShape(const ShapeClosingMode mode)
	{
	}

	void MainGraphicsLayer::Vertex(float x, float y)
	{
	}

	void MainGraphicsLayer::Background(const color_t color)
	{
		if (const std::shared_ptr<Renderer> renderer = m_Renderer.lock())
		{
			constexpr RenderingProperties properties = {
				.ClippingRect = ClipRect::Unclipped(),
				.BlendMode = BlendMode::Alpha,
				.ModelMatrix = Math::Matrix4x4::Identity,
				.IsFillEnabled = true,
				.IsStrokeEnabled = false,
			};

			// Can skip stroke properties for background fill
			constexpr StrokeProperties strokeProperties = {};

			renderer->RenderRectangle(m_RenderTarget.GetViewport(), color, {}, 0.0f, strokeProperties, properties);
		}
	}

	void MainGraphicsLayer::Rect(const float x1, const float y1, const float x2, const float y2)
	{
		const RenderStyle& style = PeekStyle();

		// If there's no fill or stroke, we can skip rendering entirely
		if (not style.IsFillEnabled and not style.IsStrokeEnabled)
		{
			return;
		}

		if (const std::shared_ptr<Renderer> renderer = m_Renderer.lock())
		{
			const RenderingProperties properties = StyleToRenderingProperties(style);
			const StrokeProperties strokeProperties = StyleToStrokeProperties(style);
			const Math::FloatBoundary boundary = style.RectMode(x1, y1, x2, y2);

			renderer->RenderRectangle(boundary, style.FillColor, style.StrokeColor, style.StrokeWeight, strokeProperties, properties);
		}
	}

	void MainGraphicsLayer::RoundedRect(const float x1, const float y1, const float x2, const float y2, const Math::BorderRadius& borderRadius)
	{
		const RenderStyle& style = PeekStyle();

		if (not style.IsFillEnabled and not style.IsStrokeEnabled)
		{
			return;
		}

		if (const std::shared_ptr<Renderer> renderer = m_Renderer.lock())
		{
			const Math::FloatBoundary boundary = style.RectMode(x1, y1, x2, y2);
			const RenderingProperties properties = StyleToRenderingProperties(style);
			renderer->RenderRoundedRectangle(boundary, borderRadius, style.FillColor, style.StrokeColor, style.StrokeWeight, properties);
		}
	}

	void MainGraphicsLayer::Ellipse(const float x1, const float y1, const float x2, const float y2)
	{
		const RenderStyle& style = PeekStyle();

		// If there's no fill or stroke, we can skip rendering entirely
		if (not style.IsFillEnabled and not style.IsStrokeEnabled)
		{
			return;
		}

		const std::shared_ptr<Renderer> renderer = m_Renderer.lock();

		const Math::FloatBoundary boundary = style.EllipseMode(x1, y1, x2, y2);
		const Math::Radius radius = Math::Radius::Elliptical(boundary.Width / 2.0f, boundary.Height / 2.0f);
		const Math::Float2 center = boundary.Center();
		const RenderingProperties properties = StyleToRenderingProperties(style);

		renderer->RenderEllipse(center, radius, style.FillColor, style.StrokeColor, style.StrokeWeight, properties);
	}

	void MainGraphicsLayer::Point(const float x, const float y)
	{
		if (const std::shared_ptr<Renderer> renderer = m_Renderer.lock())
		{
			const RenderStyle& style = PeekStyle();
			const Math::Radius radius = Math::Radius::Circular(style.StrokeWeight / 2.0f);

			RenderingProperties properties = StyleToRenderingProperties(style);
			properties.IsStrokeEnabled = false;

			renderer->RenderEllipse({ x, y }, radius, style.StrokeColor, { 0.0f, 0.0f, 0.0f, 0.0f }, 0.0f, properties);

			//renderer->FillEllipse(Math::Float2{ x, y }, radius, style.StrokeColor, segments, StyleToRenderingProperties(style));
		}
	}

	void MainGraphicsLayer::Line(const float x1, const float y1, const float x2, const float y2)
	{
		if (const std::shared_ptr<Renderer> renderer = m_Renderer.lock())
		{
			const Math::Float2 start = { x1, y1 };
			const Math::Float2 end = { x2, y2 };

			const RenderStyle& style = PeekStyle();
			const size_t segments = style.EllipseSegmentsMode(Math::Radius::Circular(style.StrokeWeight), Math::Degrees(180.0f));

			//renderer->FillLine(start, end, style.StrokeWeight, style.StrokeColor, style.StrokeCap, segments, StyleToRenderingProperties(style));
		}
	}

	void MainGraphicsLayer::Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3)
	{
		const RenderStyle& style = PeekStyle();

		// If there's no fill or stroke, we can skip rendering entirely
		if (not style.IsFillEnabled and not style.IsStrokeEnabled)
		{
			return;
		}

		const std::shared_ptr<Renderer> renderer = m_Renderer.lock();
		const RenderingProperties properties = StyleToRenderingProperties(style);

		const Math::Float2 p1{ x1, y1 };
		const Math::Float2 p2{ x2, y2 };
		const Math::Float2 p3{ x3, y3 };

	}

	void MainGraphicsLayer::Text(const std::string_view text, const float x, const float y)
	{
		const RenderStyle& style = PeekStyle();
		const std::shared_ptr<Renderer> renderer = m_Renderer.lock();

		//renderer->Text(text, *style.Font, Math::Float2{ x, y }, style.TextSize, style.FillColor, style.TextAlign, StyleToRenderingProperties(style));
	}

	void MainGraphicsLayer::Image(const Image2D& image, const float x1, const float y1, const float x2, const float y2, const float sourceLeft, const float sourceTop, const float sourceWidth, const float sourceHeight)
	{
		const RenderStyle& style = PeekStyle();
		const Math::FloatBoundary boundary = style.RectMode(x1, y1, x2, y2);
		const Math::FloatBoundary sourceBoundary = Math::FloatBoundary::FromLTWH(sourceLeft, sourceTop, sourceWidth, sourceHeight);
		const std::shared_ptr<Renderer> renderer = m_Renderer.lock();

		//renderer->Image(boundary, sourceBoundary, image, style.ImageSampler, style.ImageTint, StyleToRenderingProperties(style));
	}
}