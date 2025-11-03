// Project Name : DirectGL
// File Name    : DirectGL-BatchedQuadRenderer.ixx
// Author       : Felix Busch
// Created Date : 2025/11/03

module;

#include <glad/gl.h>

#include <memory>

export module DirectGL:BatchedQuadRenderer;

import :QuadRenderer;

namespace DGL
{
	class BatchedQuadRenderer : public QuadRenderer
	{
	public:

		explicit BatchedQuadRenderer(size_t maxQuadsPerBatch);

		~BatchedQuadRenderer() override;

		void BeginDraw(const Math::Matrix4x4& projectionViewMatrix) override;
		void EndDraw() override;
		void SubmitQuad(const QuadProperties& properties) override;

	private:

		void Flush();

		size_t m_MaxQuadsPerBatch;
		size_t m_QuadsInCurrentBatch;

		GLuint m_VertexArray;
		GLuint m_VertexBuffer;
		GLuint m_ElementBuffer;

		GLuint m_ShaderProgramId;
		GLint m_ProjectionViewMatrixLocation;

		std::unique_ptr<QuadVertex[]> m_BatchedVertices;

	};
}
