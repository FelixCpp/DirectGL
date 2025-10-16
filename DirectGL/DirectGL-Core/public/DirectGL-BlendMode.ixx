// Project Name : DirectGL-Core
// File Name    : DirectGL-BlendMode.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

export module DirectGL:BlendMode;

import DirectGL.Blending;

/// @brief This namespace wraps all blend mode related functionality of the DirectGL-Renderer library
///		   and re-exports it into the DirectGL namespace for easier access.
///
/// This pattern allows us to give fine-grained control over what parts of the
/// DirectGL-Renderer library are exposed to the users of the DirectGL library,
/// while still keeping the implementation details hidden.
export namespace DGL
{
	//using Renderer::BlendMode;
}

export namespace DGL::BlendModes
{
	using Blending::BlendModes::Alpha;
	using Blending::BlendModes::Additive;
	using Blending::BlendModes::Subtractive;
	using Blending::BlendModes::Dividing;
	using Blending::BlendModes::Multiply;
	using Blending::BlendModes::Screen;
	using Blending::BlendModes::Opaque;
	using Blending::BlendModes::Invert;
	using Blending::BlendModes::Darken;
	using Blending::BlendModes::Lighten;
	using Blending::BlendModes::Difference;
	using Blending::BlendModes::Exclusion;
}