// Project Name : DirectGL
// File Name    : DirectGL-MeshRenderer.ixx
// Author       : Felix Busch
// Created Date : 2025/11/04

module;

#include <span>
#include <vector>

export module DirectGL:MeshRenderer;

import :Mesh;
import :VertexRenderer;

namespace DGL
{
	class MeshRenderer
	{
	private:

		struct RenderableMeshVertex
		{
			Math::Float3 Position;
			Math::Float4 Color;
		};

		struct RenderableMesh
		{
			std::vector<RenderableMeshVertex> Vertices;
			std::vector<uint32_t> Indices;
		};

	public:

		MeshRenderer();

		void BeginDraw(const Math::Matrix4x4& projectionViewMatrix);
		void EndDraw();
		void Submit(const std::span<const Mesh>& meshes);

	private:

		VertexRenderer m_VertexRenderer;

	};
}