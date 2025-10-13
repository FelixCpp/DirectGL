module;

#include <cmath>

module DirectGL.Renderer;

import DirectGL.Math;

namespace DGL::Renderer
{
	Color Color::FromHSL(float hue, float saturation, float lightness, const uint8_t alpha)
	{
		hue = std::fmod(hue, 360.0f);
		if (hue < 0.0f) hue += 360.0f;

		saturation = Math::Constrain(saturation, 0.0f, 1.0f);
		lightness = Math::Constrain(lightness, 0.0f, 1.0f);

		const float c = (1.0f - std::fabs(2.0f * lightness - 1.0f)) * saturation;
		const float x = c * (1.0f - std::fabs(std::fmod(hue / 60.0f, 2) - 1.0f));
		const float m = lightness - c / 2.0f;

		float rPrime = 0.0f;
		float gPrime = 0.0f;
		float bPrime = 0.0f;

		if (hue < 60.0f) { rPrime = c; gPrime = x; bPrime = 0.0f; }
		else if (hue < 120.0f) { rPrime = x; gPrime = c; bPrime = 0.0f; }
		else if (hue < 180.0f) { rPrime = 0.0f; gPrime = c; bPrime = x; }
		else if (hue < 240.0f) { rPrime = 0.0f; gPrime = x; bPrime = c; }
		else if (hue < 300.0f) { rPrime = x; gPrime = 0.0f; bPrime = c; }
		else { rPrime = c; gPrime = 0.0f; bPrime = x; }

		return Color(
			static_cast<uint8_t>(Math::Constrain((rPrime + m) * 255.0f, 0.0f, 255.0f)),
			static_cast<uint8_t>(Math::Constrain((gPrime + m) * 255.0f, 0.0f, 255.0f)),
			static_cast<uint8_t>(Math::Constrain((bPrime + m) * 255.0f, 0.0f, 255.0f)),
			alpha
		);
	}

	Color Color::FromHSV(float hue, float saturation, float value, const uint8_t alpha)
	{
		hue = std::fmod(hue, 360.0f);
		if (hue < 0.0f) hue += 360.0f;
		saturation = Math::Constrain(saturation, 0.0f, 1.0f);
		value = Math::Constrain(value, 0.0f, 1.0f);

		const float c = value * saturation;
		const float x = c * (1.0f - std::fabs(std::fmod(hue / 60.0f, 2) - 1.0f));
		const float m = value - c;

		float rPrime = 0.0f;
		float gPrime = 0.0f;
		float bPrime = 0.0f;

		if (hue < 60.0f) { rPrime = c; gPrime = x; bPrime = 0.0f; }
		else if (hue < 120.0f) { rPrime = x; gPrime = c; bPrime = 0.0f; }
		else if (hue < 180.0f) { rPrime = 0.0f; gPrime = c; bPrime = x; }
		else if (hue < 240.0f) { rPrime = 0.0f; gPrime = x; bPrime = c; }
		else if (hue < 300.0f) { rPrime = x; gPrime = 0.0f; bPrime = c; }
		else { rPrime = c; gPrime = 0.0f; bPrime = x; }

		return Color(
			static_cast<uint8_t>(Math::Constrain((rPrime + m) * 255.0f, 0.0f, 255.0f)),
			static_cast<uint8_t>(Math::Constrain((gPrime + m) * 255.0f, 0.0f, 255.0f)),
			static_cast<uint8_t>(Math::Constrain((bPrime + m) * 255.0f, 0.0f, 255.0f)),
			alpha
		);
	}
}