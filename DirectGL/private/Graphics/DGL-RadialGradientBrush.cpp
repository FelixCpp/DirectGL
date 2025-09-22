module;

#include <cstdint>
#include <algorithm>
#include <stdexcept>

module DGL;

inline static constexpr auto VERTEX_SOURCE = R"(
#version 460 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 0) out vec2 v_TexCoord;

void main() {
	gl_Position = vec4(a_Position, 0.0, 1.0);
	v_TexCoord = a_TexCoord;
}
)";

inline static constexpr auto FRAGMENT_SOURCE = R"(
#version 460 core

#define EXTEND_MODE_CLAMP 0
#define EXTEND_MODE_WRAP 1
#define EXTEND_MODE_MIRROR 2

#define GAMMA_2_2 0
#define GAMMA_1_0 1

layout (location = 0) out vec4 o_FragColor;

layout (location = 0) in vec2 v_TexCoord;

uniform vec2 u_Center;
uniform vec2 u_Offset;
uniform vec2 u_Radius;

uniform int u_StopCount;
uniform float u_Progress[16];
uniform vec4 u_Color[16];

uniform int u_ExtendMode;
uniform int u_Gamma;

vec4 gammaMix(vec4 c1, vec4 c2, float t) {
	if (u_Gamma == GAMMA_1_0) {
		// linear mix
		return mix(c1, c2, t);
	}

	if (u_Gamma == GAMMA_2_2) {
		// sRGB gamma correction
		// https://en.wikipedia.org/wiki/SRGB#The_reverse_transformation
		
		// GAMMA2_2: sRGB -> linear -> mix -> sRGB
		vec3 c1_lin = pow(c1.rgb, vec3(2.2));
		vec3 c2_lin = pow(c2.rgb, vec3(2.2));
		vec3 mixed = mix(c1_lin, c2_lin, t);
		return vec4(pow(mixed, vec3(1.0/2.2)), mix(c1.a, c2.a, t));
	}

	return vec4(1.0, 0.0, 1.0, 1.0); // error: magenta
}

void main() {
	vec2 pos = v_TexCoord - (u_Center + u_Offset);
	float dist = length(pos) / u_Radius.x;

	if (u_ExtendMode == EXTEND_MODE_CLAMP) {
		dist = clamp(dist, 0.0, 1.0);
	} else if (u_ExtendMode == EXTEND_MODE_WRAP) {
		dist = fract(dist);
	} else if (u_ExtendMode == EXTEND_MODE_MIRROR) {
		dist = abs(fract(dist * 0.5) * 2.0 - 1.0);
	}

	// In case the distance is before the first stop, use the first color
	if (dist <= u_Progress[0]) {
		o_FragColor = u_Color[0];
		return;
	}

	// In case the distance is after the last stop, use the last color
	if (dist >= u_Progress[u_StopCount-1]) {
		o_FragColor = u_Color[u_StopCount-1];
		return;
	}

	// Find the two stops the distance is in between and interpolate the color
	for (int i = 0; i < u_StopCount - 1; ++i) {
		if (dist >= u_Progress[i] && dist <= u_Progress[i+1]) {
			float factor = (dist - u_Progress[i]) / (u_Progress[i+1] - u_Progress[i]);
			o_FragColor = gammaMix(u_Color[i], u_Color[i+1], factor);
			return;
		}
	}
	
	o_FragColor = vec4(0.0);
}
)";

namespace DGL
{
	static constexpr std::int32_t GetExtendModeId(const ExtendMode extendMode)
	{
		switch (extendMode)
		{
			using enum ExtendMode;
			case Clamp: return 0;
			case Wrap: return 1;
			case Mirror: return 2;
			default: throw std::invalid_argument("Unknown ExtendMode");
		}
	}

	static constexpr std::int32_t GetGammaModeId(const Gamma gamma)
	{
		switch (gamma)
		{
			using enum Gamma;
			case Gamma2_2: return 0;
			case Gamma1_0: return 1;
			default: throw std::invalid_argument("Unknown Gamma");
		}
	}

	RadialGradientBrush::RadialGradientBrush(const std::vector<GradientStop>& stops, const ExtendMode extendMode, const Gamma gamma, const Math::Float2& center, const Math::Float2& offset, const Math::Float2& radius):
		m_ExtendMode(extendMode),
		m_Gamma(gamma),
		m_Center(center),
		m_Offset(offset),
		m_Radius(radius)
	{
		m_StopCount = std::min(stops.size(), 16uz);
		for (std::size_t i = 0; i < m_StopCount; ++i)
		{
			m_StopProgress[i] = stops[i].Progress;
			m_StopColor[i * 4 + 0] = stops[i].Red;
			m_StopColor[i * 4 + 1] = stops[i].Green;
			m_StopColor[i * 4 + 2] = stops[i].Blue;
			m_StopColor[i * 4 + 3] = stops[i].Alpha;
		}

		Shader vertexShader;
		if (not vertexShader.LoadFromMemory(VERTEX_SOURCE, Shader::Type::Vertex))
		{
			Error("Failed to load vertex shader for radial gradient brush.");
		}

		Shader fragmentShader;
		if (not fragmentShader.LoadFromMemory(FRAGMENT_SOURCE, Shader::Type::Fragment))
		{
			Error("Failed to load fragment shader for radial gradient brush.");
		}

		if (not m_ShaderProgram.Load(vertexShader, fragmentShader))
		{
			Error("Failed to create shader program for radial gradient brush.");
		}
	}

	void RadialGradientBrush::Apply()
	{
		m_ShaderProgram.Bind();
		m_ShaderProgram.UploadFloat2("u_Center", m_Center.X, m_Center.Y);
		m_ShaderProgram.UploadFloat2("u_Offset", m_Offset.X, m_Offset.Y);
		m_ShaderProgram.UploadFloat2("u_Radius", m_Radius.X, m_Radius.Y);

		m_ShaderProgram.UploadInt1("u_StopCount", m_StopCount);
		m_ShaderProgram.UploadFloat1Array("u_Progress", m_StopProgress.data(), m_StopCount);
		m_ShaderProgram.UploadFloat4Array("u_Color", m_StopColor.data(), m_StopCount);

		m_ShaderProgram.UploadInt1("u_ExtendMode", GetExtendModeId(m_ExtendMode));
		m_ShaderProgram.UploadInt1("u_Gamma", GetGammaModeId(m_Gamma));
	}
}