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
		for (const auto& [positions, colors, indices] : meshes)
		{
			// For each mesh we need to gather the positions and colors,
			// transforming them into our RenderableMeshVertex format
			// and storing them in the staging buffer.
			// in order to upload them to the GPU for rendering later on.

			// After we've transformed the mesh, we can submit it to the VertexRenderer
			m_VertexRenderer.Submit(std::span(positions), std::span(colors), std::span(indices));
		}
	}
}