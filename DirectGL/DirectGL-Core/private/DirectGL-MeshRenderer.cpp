module;

#include <ranges>

module DirectGL;

import :MeshRenderer;

namespace DGL
{
	MeshRenderer::MeshRenderer()
	{
	}

	void MeshRenderer::BeginDraw(const Math::Matrix4x4& projectionViewMatrix)
	{
		m_VertexRenderer.BeginDraw(projectionViewMatrix);
	}

	void MeshRenderer::EndDraw()
	{
		m_VertexRenderer.EndDraw();
	}

	void MeshRenderer::Submit(const std::span<const Mesh>& meshes)
	{
		for (const auto& [vertices, indices, depth] : meshes)
		{
			// For each mesh we need to gather the positions and colors,
			// transforming them into our RenderableMeshVertex format
			// and storing them in the staging buffer.
			// in order to upload them to the GPU for rendering later on.

			std::vector<Math::Float3> positions;
			std::vector<Math::Float4> colors;

			for (const auto& [position, color] : vertices)
			{
				positions.push_back({ position.X, position.Y, depth });
				colors.push_back(color);
			}

			// After we've transformed the mesh, we can submit it to the VertexRenderer
			m_VertexRenderer.Submit(std::span(positions), std::span(colors), std::span(indices));
		}
	}
}