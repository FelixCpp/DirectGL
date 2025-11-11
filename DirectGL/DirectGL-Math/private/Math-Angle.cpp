module DirectGL.Math;

import :Random;

namespace DGL::Math
{
	Angle Angle::Random()
	{
		return Degrees(Math::Random(360.0f));
	}
}