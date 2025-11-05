module;

#include <cmath>
#include <algorithm>

module DirectGL;

import :EllipseSegmentsMode;

namespace DGL
{
	const EllipseSegmentsMode& EllipseSegmentsModeFixed(size_t count)
	{
		static EllipseSegmentsMode mode = [count](Math::Radius, Math::Angle) {
			return count;
		};

		return mode;
	}

	const EllipseSegmentsMode& EllipseSegmentsModeAdaptive(float tolerance)
	{
		static EllipseSegmentsMode mode = [tolerance](const Math::Radius radius, const Math::Angle sweep) {
			const float targetRadius = radius.Max();
			const float sweepRadians = sweep.AsRadians();

			const float da = std::acosf(targetRadius / (targetRadius + tolerance)) * 2.0f;
			const float segmentCount = std::ceilf(sweepRadians / da);
			return std::max(segmentCount, 3.0f);
		};

		return mode;
	}
}