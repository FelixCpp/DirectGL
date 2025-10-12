module;

#include <random>

module DirectGL.Math;

namespace DGL::Math
{
	thread_local std::mt19937 engine{ std::random_device{}() };
	thread_local std::uniform_real_distribution distribution{ 0.0f, 1.0f };

	void SetRNGSeed(const uint32_t seed)
	{
		engine.seed(seed);
	}

	float Random(const float min, const float max)
	{
		return min + (max - min) * distribution(engine);
	}

	float Random(const float max)
	{
		return Random(0.0f, max);
	}
}