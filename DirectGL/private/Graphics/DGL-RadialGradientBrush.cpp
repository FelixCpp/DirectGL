module;

#include <cstdint>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <ranges>

module DGL;

inline static constexpr auto VERTEX_SOURCE = R"(
#version 460 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 0) out vec2 v_TexCoord;

mat4 getProjectionMatrix(in float left, in float top, in float right, in float bottom) {
	const float near = -1.0;
	const float far = 1.0;
	const float rl = right - left;
	const float tb = top - bottom;
	const float fn = far - near;

	/*return mat4(
		2.0 / rl, 0.0, 0.0, -(right + left) / rl,
		0.0, 2.0 / tb, 0.0, -(top + bottom) / tb,
		0.0, 0.0, -2.0 / fn, -(far + near) / fn,
		0.0, 0.0, 0.0, 1.0
	);*/

	return mat4(
		2.0 / rl, 0.0, 0.0, 0.0,
		0.0, 2.0 / tb, 0.0, 0.0,
		0.0, 0.0, -2.0 / fn, 0.0,
		-(right + left) / rl, -(top + bottom) / tb, -(far + near) / fn, 1.0
	);
}

void main() {
	gl_Position = getProjectionMatrix(0.0, 0.0, 900.0, 900.0) * vec4(a_Position, 0.0, 1.0);
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

layout (std140, binding = 0) uniform GradientData {
	vec4 u_CenterOffset;
	vec4 u_RadiusExtendGamma;
	vec4 u_Progress[16];
	vec4 u_Color[16];
	int u_StopCount;
};

vec4 gammaMix(int gamma, vec4 c1, vec4 c2, float t) {
	if (gamma == GAMMA_1_0) {
		// linear mix
		return mix(c1, c2, t);
	}

	if (gamma == GAMMA_2_2) {
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
	vec2 center = u_CenterOffset.xy;
	vec2 offset = u_CenterOffset.zw;
	vec2 radius = u_RadiusExtendGamma.xy;

	vec2 pos = v_TexCoord - (center + offset);
	vec2 norm = pos / radius;
	float dist = length(norm);

	int extendMode = int(u_RadiusExtendGamma.z);
	int gammaMode = int(u_RadiusExtendGamma.w);
	if (extendMode == EXTEND_MODE_CLAMP) {
		dist = clamp(dist, 0.0, 1.0);
	} else if (extendMode == EXTEND_MODE_WRAP) {
		dist = fract(dist);
	} else if (extendMode == EXTEND_MODE_MIRROR) {
		dist = abs(fract(dist * 0.5) * 2.0 - 1.0);
	}

	// In case the distance is before the first stop, use the first color
	if (dist <= u_Progress[0].x) {
		o_FragColor = u_Color[0];
		return;
	}

	// In case the distance is after the last stop, use the last color
	if (dist >= u_Progress[u_StopCount-1].x) {
		o_FragColor = u_Color[u_StopCount-1];
		return;
	}

	// Find the two stops the distance is in between and interpolate the color
	for (int i = 0; i < u_StopCount - 1; ++i) {
		if (dist >= u_Progress[i].x && dist <= u_Progress[i+1].x) {
			float factor = (dist - u_Progress[i].x) / (u_Progress[i+1].x - u_Progress[i].x);
			o_FragColor = gammaMix(gammaMode, u_Color[i], u_Color[i+1], factor);
			return;
		}
	}
	
	o_FragColor = vec4(1.0, 0.0, 1.0, 1.0); // error: magenta
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

	static constexpr std::int32_t GetGammaModeId(const GammaMode gammaMode)
	{
		switch (gammaMode)
		{
			using enum GammaMode;
			case Gamma2_2: return 0;
			case Gamma1_0: return 1;
			default: throw std::invalid_argument("Unknown GammaMode");
		}
	}

	std::unique_ptr<RadialGradientBrush> RadialGradientBrush::Create(const Properties& properties) {
		const auto vertexShader = Shader::CreateFromSource(VERTEX_SOURCE, Shader::Type::Vertex);
		if (vertexShader == nullptr)
		{
			Error("Failed to load vertex shader for radial gradient brush.");
			return nullptr;
		}

		const auto fragmentShader = Shader::CreateFromSource(FRAGMENT_SOURCE, Shader::Type::Fragment);
		if (fragmentShader == nullptr)
		{
			Error("Failed to load fragment shader for radial gradient brush.");
			return nullptr;
		}

		auto shaderProgram = ShaderProgram::Create(*vertexShader, *fragmentShader);
		if (shaderProgram == nullptr)
		{
			Error("Failed to create shader program for radial gradient brush.");
			return nullptr;
		}

		return std::unique_ptr<RadialGradientBrush>(new RadialGradientBrush(std::move(shaderProgram), properties));
	}

	void RadialGradientBrush::Apply()
	{
		m_ShaderProgram->Bind();
		m_UniformBuffer->Bind();
		m_UniformBuffer->Upload(&m_Properties, sizeof(Std140Properties));
	}

	RadialGradientBrush::RadialGradientBrush(
		std::unique_ptr<ShaderProgram> shaderProgram,
		const Properties& properties
	) : m_ShaderProgram(std::move(shaderProgram)),
		m_UniformBuffer(std::make_unique<UniformBuffer>(nullptr, sizeof(Std140Properties))),
		m_Properties(PropertiesToStd140(properties))
	{
	}

	RadialGradientBrush::Std140Properties RadialGradientBrush::PropertiesToStd140(const Properties& properties)
	{
		const auto stopCount = static_cast<int>(std::min(properties.GradientStops.size(), 16uz));
		std::array<Math::Float4, 16> positions;
		std::array<Math::Float4, 16> colors;
		for (int i = 0; i < stopCount; ++i)
		{
			const auto& stop = properties.GradientStops[i];
			positions[i] = { stop.Position, 0.0f, 0.0f, 0.0f };
			colors[i] = { stop.Color.R / 255.0f, stop.Color.G / 255.0f, stop.Color.B / 255.0f, stop.Color.A / 255.0f };
		}

		return {
			.CenterOffset = {
				properties.Center.X,
				properties.Center.Y,
				properties.Offset.X,
				properties.Offset.Y
			},
			.RadiusExtendGamma = {
				properties.Radius.X,
				properties.Radius.Y,
				static_cast<float>(GetExtendModeId(properties.ExtendMode)),
				static_cast<float>(GetGammaModeId(properties.GammaMode))
			},
			.Positions = positions,
			.Colors = colors,
			.StopCount = stopCount,
		};
	}

}