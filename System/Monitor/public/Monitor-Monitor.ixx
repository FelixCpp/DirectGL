module;

#include <string>

export module System.Monitor:Monitor;

import DirectGL.Math;

export namespace System
{
	struct Monitor
	{
		std::string Name;
		DGL::Math::IntBoundary WorkArea;
		DGL::Math::IntBoundary Area;
		bool IsPrimary;
	};
}