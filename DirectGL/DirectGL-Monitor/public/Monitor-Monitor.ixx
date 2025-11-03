module;

#include <string>

export module DirectGL.Monitor:Monitor;

import DirectGL.Math;

export namespace DGL
{
	struct Monitor
	{
		std::string Name;
		Math::IntBoundary WorkArea;
		Math::IntBoundary Area;
		bool IsPrimary;
	};
}