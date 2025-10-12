// Project Name : DirectGL-Core
// File Name    : DirectGL-BlendMode.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

export module DirectGL:BlendMode;

import DirectGL.Renderer;

/// @brief This namespace wraps all blend mode related functionality of the DirectGL-Renderer library
///		   and re-exports it into the DirectGL namespace for easier access.
///
/// This pattern allows us to give fine-grained control over what parts of the
/// DirectGL-Renderer library are exposed to the users of the DirectGL library,
/// while still keeping the implementation details hidden.
export namespace DGL
{
	using Renderer::BlendMode;
}

export namespace DGL::BlendModes
{
	using Renderer::BlendModes::Alpha;
	using Renderer::BlendModes::Additive;
	using Renderer::BlendModes::Subtractive;
	using Renderer::BlendModes::Dividing;
	using Renderer::BlendModes::Multiply;
	using Renderer::BlendModes::Screen;
	using Renderer::BlendModes::Opaque;
	using Renderer::BlendModes::Invert;
	using Renderer::BlendModes::Darken;
	using Renderer::BlendModes::Lighten;
	using Renderer::BlendModes::Difference;
	using Renderer::BlendModes::Exclusion;
}