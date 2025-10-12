// Project Name : Math
// File Name    : Math-Random.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

module;

#include <cstdint>

export module DirectGL.Math:Random;

export namespace DGL::Math
{
	void SetRNGSeed(uint32_t seed);
	float Random(float min, float max);
	float Random(float max);
}