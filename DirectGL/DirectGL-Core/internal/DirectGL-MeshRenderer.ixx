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
import :BlendMode;

namespace DGL
{
	class MeshRenderer
	{
	public:

		MeshRenderer();

		void BeginDraw(const Math::Matrix4x4& projectionViewMatrix);
		void EndDraw();
		void Submit(const std::span<const Mesh>& meshes, const BlendMode& blendMode, const Math::Matrix4x4& modelMatrix);
		void Submit(const Mesh& mesh, const BlendMode& blendMode, const Math::Matrix4x4& modelMatrix);

	private:

		VertexRenderer m_VertexRenderer;

	};
}