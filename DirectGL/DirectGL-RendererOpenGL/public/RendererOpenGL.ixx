// Project Name : DirectGL-RendererOpenGL
// File Name    : RendererOpenGL.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

export module DirectGL.RendererOpenGL;

export import :TextureSampler;
export import :ShaderProgram;

import DirectGL.LibraryLogger;

export namespace DGL::RendererOpenGL
{
	void SetLogger(Logger* logger);
}

module :private;

DGL::LibraryLogger::LibraryLogger s_Logger;

namespace DGL::RendererOpenGL
{
	void SetLogger(Logger* logger)
	{
		s_Logger.SetLogger(logger);
	}
}
