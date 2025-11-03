module;

#include <optional>

module DirectGL;

import Preconditions;

import :ShapeBuilder;

namespace DGL
{
	ShapeBuilder::ShapeBuilder(QuadRenderer& quadRenderer):
		m_CurrentShapeMode(std::nullopt),
		m_QuadRenderer(&quadRenderer)
	{
	}

	void ShapeBuilder::Begin(const ShapeMode mode)
	{
		m_CurrentShapeMode = mode;
	}

	void ShapeBuilder::End()
	{
		// Check if there are vertices to process
		if (m_Vertices.empty())
		{
			return;
		}

		switch (m_CurrentShapeMode.value())
		{
			case ShapeMode::Points: UploadVerticesAsPoints(); break;
			case ShapeMode::Lines: UploadVerticesAsLines(); break;
			case ShapeMode::Triangles: UploadVerticesAsTriangles(); break;
			case ShapeMode::TriangleStrip: UploadVerticesAsTriangleStrip(); break;
			case ShapeMode::TriangleFan: UploadVerticesAsTriangleFan(); break;
			case ShapeMode::Quads: UploadVerticesAsQuads(); break;
			case ShapeMode::QuadStrip: UploadVerticesAsQuadStrip(); break;
		}

		m_CurrentShapeMode.reset();
		m_Vertices.clear();
	}

	void ShapeBuilder::AddVertex(const ShapeVertex& vertex)
	{
		if (not m_CurrentShapeMode.has_value())
		{
			// TODO(Felix): Print out a warning that no shape has been started
			return;
		}

		m_Vertices.emplace_back(vertex);
	}

	void ShapeBuilder::UploadVerticesAsPoints()
	{
		for (size_t i = 0; i < m_Vertices.size(); ++i)
		{
			const ShapeVertex& v = m_Vertices[i];
			System::TODO("Points are not yet implemented");
		}
	}

	void ShapeBuilder::UploadVerticesAsLines()
	{
		for (size_t i = 1; i < m_Vertices.size(); i += 2)
		{
			const ShapeVertex& v1 = m_Vertices[i - 1];
			const ShapeVertex& v2 = m_Vertices[i - 0];
			System::TODO("Lines are not yet implemented");
		}
	}

	void ShapeBuilder::UploadVerticesAsTriangles()
	{
		for (size_t i = 2; i < m_Vertices.size(); i += 3)
		{
			const ShapeVertex& v1 = m_Vertices[i - 3];
			const ShapeVertex& v2 = m_Vertices[i - 2];
			const ShapeVertex& v3 = m_Vertices[i - 1];
			System::TODO("Triangles are not yet implemented");
		}
	}

	void ShapeBuilder::UploadVerticesAsTriangleStrip()
	{
		for (size_t i = 2; i < m_Vertices.size(); ++i)
		{
			const ShapeVertex& v1 = m_Vertices[i - 2];
			const ShapeVertex& v2 = m_Vertices[i - 1];
			const ShapeVertex& v3 = m_Vertices[i - 0];
			System::TODO("Triangle strips are not yet implemented");
		}
	}

	void ShapeBuilder::UploadVerticesAsTriangleFan()
	{
		for (size_t i = 2; i < m_Vertices.size(); ++i)
		{
			const ShapeVertex& v1 = m_Vertices[0];
			const ShapeVertex& v2 = m_Vertices[i - 1];
			const ShapeVertex& v3 = m_Vertices[i - 0];
			System::TODO("Triangle fans are not yet implemented");
		}
	}

	void ShapeBuilder::UploadVerticesAsQuads() const
	{
		for (size_t i = 3; i < m_Vertices.size(); i += 4)
		{
			const ShapeVertex& v1 = m_Vertices[i - 3];
			const ShapeVertex& v2 = m_Vertices[i - 2];
			const ShapeVertex& v3 = m_Vertices[i - 1];
			const ShapeVertex& v4 = m_Vertices[i - 0];

			m_QuadRenderer->SubmitQuad({
				CreateQuadProperties(v1, v2, v3, v4),
			});
		}
	}

	void ShapeBuilder::UploadVerticesAsQuadStrip() const
	{
		for (size_t i = 3; i < m_Vertices.size(); i += 2)
		{
			const ShapeVertex& v1 = m_Vertices[i - 3];
			const ShapeVertex& v2 = m_Vertices[i - 2];
			const ShapeVertex& v3 = m_Vertices[i - 1];
			const ShapeVertex& v4 = m_Vertices[i - 0];

			m_QuadRenderer->SubmitQuad({
				CreateQuadProperties(v1, v2, v3, v4),
			});
		}
	}


	QuadRenderer::QuadProperties ShapeBuilder::CreateQuadProperties(const ShapeVertex& v1, const ShapeVertex& v2, const ShapeVertex& v3, const ShapeVertex& v4)
	{
		// TODO(Felix): Compute size based on vertex positions
		const float width = v2.Position.X - v1.Position.X;
		const float height = v4.Position.Y - v1.Position.Y;

		return {
			QuadRenderer::QuadVertex{
				.WorldPosition = v1.Position,
				.LocalPosition = { 0.0f, 0.0f, },
				.FillColor = v1.FillColor,
				.StrokeColor = v1.StrokeColor,
				.Size = { width, height },
				.StrokeWeight = v1.StrokeWeight,
				.BorderRadius = 0.0f,
			},
			QuadRenderer::QuadVertex{
				.WorldPosition = v2.Position,
				.LocalPosition = { 1.0f, 0.0f, },
				.FillColor = v2.FillColor,
				.StrokeColor = v2.StrokeColor,
				.Size = { width, height },
				.StrokeWeight = v2.StrokeWeight,
				.BorderRadius = 0.0f,
			},
			QuadRenderer::QuadVertex{
				.WorldPosition = v3.Position,
				.LocalPosition = { 1.0f, 1.0f, },
				.FillColor = v3.FillColor,
				.StrokeColor = v3.StrokeColor,
				.Size = { width, height },
				.StrokeWeight = v3.StrokeWeight,
				.BorderRadius = 0.0f,
			},
			QuadRenderer::QuadVertex{
				.WorldPosition = v4.Position,
				.LocalPosition = { 0.0f, 1.0f, },
				.FillColor = v4.FillColor,
				.StrokeColor = v4.StrokeColor,
				.Size = { width, height },
				.StrokeWeight = v4.StrokeWeight,
				.BorderRadius = 0.0f,
			},
		};
	}

}