module;

#include <cmath>
#include <algorithm>

module DirectGL;

import :EllipseSegmentsMode;

namespace DGL
{
	EllipseSegmentsMode EllipseSegmentsModeFixed(size_t count)
	{
		return [count](Math::Radius, Math::Angle) {
			return count;
		};
	}

	EllipseSegmentsMode EllipseSegmentsModeAdaptive(float tolerance)
	{
		static EllipseSegmentsMode mode = [tolerance](const Math::Radius radius, const Math::Angle sweep) {
			const float targetRadius = radius.Max();
			const float sweepRadians = sweep.AsRadians();

			const float da = std::acosf(targetRadius / (targetRadius + tolerance)) * 2.0f;
			const float segmentCount = std::ceilf(sweepRadians / da);
			return static_cast<size_t>(std::max(segmentCount, 3.0f));
		};

		return mode;
	}
}