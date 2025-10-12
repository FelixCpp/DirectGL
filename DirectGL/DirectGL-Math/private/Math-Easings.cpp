module;

#include <cmath>

module DirectGL.Math;

import :Constants;

namespace DGL::Math::Easings
{
	inline float Cos(const float x) { return std::cosf(x); }
	inline float Sin(const float x) { return std::sinf(x); }
	inline float Pow(const float x, const float e) { return std::powf(x, e); }

	float EaseInSine(const float x) { return 1.0f - Cos((x * PI) / 2.0f); }
	float EaseOutSine(const float x) { return Sin((x * PI) / 2.0f); }
	float EaseInOutSine(const float x) { return -(Cos(PI * x) - 1.0f) / 2.0f; }

	float EaseInCubic(const float x) { return x * x * x; }
	float EaseOutCubic(const float x) { return 1.0f - Pow(1.0f - x, 3); }
	float EaseInOutCubic(const float x) { return (x < 0.5f) ? 4.0f * x * x * x : 1.0f - Pow(-2.0f * x + 2.0f, 3) / 2.0f; }

	float EaseInQuint(const float x) { return x * x * x * x * x; }
	float EaseOutQuint(const float x) { return 1.0f - Pow(1.0f - x, 5); }
	float EaseInOutQuint(const float x) { return (x < 0.5f) ? 16.0f * x * x * x * x * x : 1.0f - Pow(-2.0f * x + 2.0f, 5) / 2.0f; }

	float EaseInCirc(const float x) { return 1.0f - std::sqrt(1.0f - Pow(x, 2)); }
	float EaseOutCirc(const float x) { return std::sqrt(1.0f - Pow(x - 1.0f, 2)); }
	float EaseInOutCirc(const float x) { return (x < 0.5f) ? (1.0f - std::sqrt(1.0f - Pow(2.0f * x, 2))) / 2.0f : (std::sqrt(1.0f - Pow(-2.0f * x + 2.0f, 2)) + 1.0f) / 2.0f; }

	float EaseInElastic(const float x) 
	{ 
		constexpr float c4 = (2.0f * PI) / 3.0f;
		return (x == 0.0f) ? 0.0f : (x == 1.0f) ? 1.0f : -Pow(2.0f, 10.0f * x - 10.0f) * Sin((x * 10.0f - 10.75f) * c4); 
	}
	float EaseOutElastic(const float x) 
	{ 
		constexpr float c4 = (2.0f * PI) / 3.0f;
		return (x == 0.0f) ? 0.0f : (x == 1.0f) ? 1.0f : Pow(2.0f, -10.0f * x) * Sin((x * 10.0f - 0.75f) * c4) + 1.0f; 
	}
	float EaseInOutElastic(const float x) 
	{ 
		constexpr float c5 = (2.0f * PI) / 4.5f;
		return (x == 0.0f) ? 0.0f : (x == 1.0f) ? 1.0f : (x < 0.5f) ? -(Pow(2.0f, 20.0f * x - 10.0f) * Sin((20.0f * x - 11.125f) * c5)) / 2.0f : (Pow(2.0f, -20.0f * x + 10.0f) * Sin((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f; 
	}

	float EaseInQuad(const float x) { return x * x; }
	float EaseOutQuad(const float x) { return 1.0f - (1.0f - x) * (1.0f - x); }
	float EaseInOutQuad(const float x) { return (x < 0.5f) ? 2.0f * x * x : 1.0f - Pow(-2.0f * x + 2.0f, 2) / 2.0f; }

	float EaseInQuart(const float x) { return x * x * x * x; }
	float EaseOutQuart(const float x) { return 1.0f - Pow(1.0f - x, 4); }
	float EaseInOutQuart(const float x) { return (x < 0.5f) ? 8.0f * x * x * x * x : 1.0f - Pow(-2.0f * x + 2.0f, 4) / 2.0f; }

	float EaseInExpo(const float x) { return (x == 0.0f) ? 0.0f : Pow(2.0f, 10.0f * x - 10.0f); }
	float EaseOutExpo(const float x) { return (x == 1.0f) ? 1.0f : 1.0f - Pow(2.0f, -10.0f * x); }
	float EaseInOutExpo(const float x) { return (x == 0.0f) ? 0.0f : (x == 1.0f) ? 1.0f : (x < 0.5f) ? Pow(2.0f, 20.0f * x - 10.0f) / 2.0f : (2.0f - Pow(2.0f, -20.0f * x + 10.0f)) / 2.0f; }

	float EaseInBack(const float x) 
	{ 
		constexpr float c1 = 1.70158f;
		constexpr float c3 = c1 + 1.0f;
		return c3 * x * x * x - c1 * x * x; 
	}

	float EaseOutBack(const float x) 
	{ 
		constexpr float c1 = 1.70158f;
		constexpr float c3 = c1 + 1.0f;
		return 1.0f + c3 * Pow(x - 1.0f, 3) + c1 * Pow(x - 1.0f, 2); 
	}

	float EaseInOutBack(const float x) 
	{ 
		constexpr float c1 = 1.70158f;
		constexpr float c2 = c1 * 1.525f;
		return (x < 0.5f) ? (Pow(2.0f * x, 2) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f : (Pow(2.0f * x - 2.0f, 2) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f; 
	}

	float EaseInBounce(const float x) { return 1.0f - EaseOutBounce(1.0f - x); }
	float EaseOutBounce(const float x) {
		if (x < 1.0f / 2.75f)
		{
			return 7.5625f * x * x;
		}

		if (x < 2.0f / 2.75f)
		{
			const float t = x - (1.5f / 2.75f);
			return 7.5625f * t * t + 0.75f;
		}

		if (x < 2.5f / 2.75f)
		{
			const float t = x - (2.25f / 2.75f);
			return 7.5625f * t * t + 0.9375f;
		}
		
		const float t = x - (2.625f / 2.75f);
		return 7.5625f * t * t + 0.984375f;
	}

	float EaseInOutBounce(const float x) 
	{ 
		return (x < 0.5f) ? (1.0f - EaseOutBounce(1.0f - 2.0f * x)) / 2.0f : (1.0f + EaseOutBounce(2.0f * x - 1.0f)) / 2.0f; 
	}
}
