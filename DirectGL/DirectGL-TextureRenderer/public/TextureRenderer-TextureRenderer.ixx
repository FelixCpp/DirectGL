// Project Name : DirectGL-TextureRenderer
// File Name    : TextureRenderer-TextureRenderer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/16

module;

#include <Glad/gl.h>

#include <memory>

export module DirectGL.TextureRenderer:TextureRenderer;

export namespace DGL::TextureRenderer
{
	class TextureRenderer
	{
	public:

		static std::unique_ptr<TextureRenderer> Create(size_t maxTexturesPerBatch);

		~TextureRenderer();

		void Render(float left, float top, float width, float height, float depth);

		void BeginDraw();
		void EndDraw();

	private:

		void Flush();

		explicit TextureRenderer(
			GLuint vertexArrayId,
			GLuint positionBufferId,
			GLuint texCoordBufferId,
			GLuint indexBufferId,
			size_t maxTexturesPerBatch
		);


		GLuint m_VertexArrayId;
		GLuint m_PositionBufferId;
		GLuint m_TexCoordBufferId;
		GLuint m_IndexBufferId;

		std::unique_ptr<float[]> m_BatchedPositions;
		size_t m_BatchedTextureCount;
		size_t m_MaxTexturesPerBatch;

	};
}