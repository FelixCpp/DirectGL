module;

#include <cmath>
#include <algorithm>
#include <stdexcept>

module DirectGL;

import DirectGL.ControlFlow;

import :Stroker;

namespace
{
	constexpr float sign(const float x)
	{
		return (x > 0.0f) ? 1.0f : ((x < 0.0f) ? -1.0f : 0.0f);
	}
}

namespace DGL
{
	Stroke PolygonStroker::GenerateStroke(
		const std::span<const Math::Float2>& points,
		const std::span<const Math::Float4>& colors,
		const float strokeWeight,
		float miterLimit,
		StrokeAlignment alignment,
		StrokeCap strokeCap,
		StrokeJoin join,
		const bool isClosed
	) {
		// This implementation assumes the number of points and colors to be the same.
		// In a production implementation, you would want to add error handling for mismatched sizes
		// One way to handle this could be to use the last color for any extra points if sizes differ.

		const size_t pointCount = points.size();
		const float halfStrokeWeight = strokeWeight * 0.5f;

		std::vector<Math::Float2> directions;
		std::vector<Math::Float2> normals;

		for (size_t i = 0; i < pointCount - 1; ++i)
		{
			const Math::Float2 delta = points[i] - points[i + 1];
			const Math::Float2 direction = delta.Normalized();
			const Math::Float2 normal = direction.Perpendicular();

			directions.emplace_back(direction);
			normals.emplace_back(normal);
		}

		if (isClosed)
		{
			// Close the loop by connecting the last point to the first
			const Math::Float2 delta = points[pointCount - 1] - points[0];
			const Math::Float2 direction = delta.Normalized();
			const Math::Float2 normal = direction.Perpendicular();
			directions.emplace_back(direction);
			normals.emplace_back(normal);
		} else
		{
			// For open shapes, duplicate the last direction and normal
			const Math::Float2 direction = directions[pointCount - 2];
			const Math::Float2 normal = normals[pointCount - 2];
			directions.emplace_back(direction);
			normals.emplace_back(normal);
		}

		Stroke stroke;

		for (size_t i = 0; i < pointCount; ++i)
		{
			// Gather some context about the current point
			const bool isStart					= (i == 0);
			const bool isEnd					= (i == pointCount - 1);
			const bool hasPreviousPoint			= isStart ? isClosed : true;
			const bool hasNextPoint				= isEnd ? isClosed : true;
			const Math::Float2 currentPoint	= points[i];

			const size_t previousIndex	= isStart ? (isClosed ? pointCount - 1 : 0) : i - 1;
			const size_t nextIndex		= isEnd ? (isClosed ? 0 : pointCount - 1) : i + 1;

			const Math::Float2 previousDirection	= directions[previousIndex];
			const Math::Float2 nextDirection		= directions[i];
			const Math::Float2 previousNormal		= normals[previousIndex];
			const Math::Float2 nextNormal			= normals[i];

			if (hasNextPoint)
			{
				// Build a line segment to the next point.
				// We can do this for all points except the last one in an open shape.
				const Math::Float2 nextPoint = points[nextIndex];
				const Math::Float2 segmentNormal = nextNormal;
				const Math::Float2 offset = segmentNormal * halfStrokeWeight;
			
				const size_t baseIndex = stroke.Vertices.size();
				stroke.Vertices.emplace_back(StrokeVertex{ .Position = currentPoint + offset, .Color = colors[i] });
				stroke.Vertices.emplace_back(StrokeVertex{ .Position = nextPoint + offset, .Color = colors[nextIndex] });
				stroke.Vertices.emplace_back(StrokeVertex{ .Position = nextPoint - offset, .Color = colors[nextIndex] });
				stroke.Vertices.emplace_back(StrokeVertex{ .Position = currentPoint - offset, .Color = colors[i] });
			
				stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 0));
				stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 1));
				stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 2));
				stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 2));
				stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 3));
				stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 0));
			}

			if (hasPreviousPoint and hasNextPoint)
			{
				// We need to handle the join between the previous and next segments
				const float cross = previousDirection.Cross(nextDirection);
				const float dot = previousDirection.Dot(nextDirection);
				const float angle = std::acos(dot);
				const Math::Float2 bisector = (previousNormal + nextNormal).Normalized();



				if ((join == StrokeJoin::Miter) or (join == StrokeJoin::Bevel))
				{
					float denominator = bisector.Dot(previousNormal);
					if (std::abs(denominator) < 1e-6f) { denominator = 1e-6f * sign(denominator); }
					const float miterLength = halfStrokeWeight / denominator;
					const bool isMiterTooLong = (std::abs(miterLength) / halfStrokeWeight > miterLimit);

					if (join == StrokeJoin::Bevel or isMiterTooLong)
					{
						const Math::Float2 outerPrevious = currentPoint + previousNormal * halfStrokeWeight;
						const Math::Float2 outerNext = currentPoint + nextNormal * halfStrokeWeight;

						const size_t baseIndex = stroke.Vertices.size();
						stroke.Vertices.emplace_back(StrokeVertex{ .Position = currentPoint, .Color = colors[i] });
						stroke.Vertices.emplace_back(StrokeVertex{ .Position = outerPrevious, .Color = colors[i] });
						stroke.Vertices.emplace_back(StrokeVertex{ .Position = outerNext, .Color = colors[i] });
						stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 0));
						stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 1));
						stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 2));
					} else if (join == StrokeJoin::Miter)
					{
						const Math::Float2 outer = currentPoint + bisector * miterLength;
						const Math::Float2 outerPrevious = currentPoint + previousNormal * halfStrokeWeight;
						const Math::Float2 outerNext = currentPoint + nextNormal * halfStrokeWeight;

						const size_t baseIndex = stroke.Vertices.size();
						stroke.Vertices.emplace_back(StrokeVertex{ .Position = outerPrevious, .Color = colors[i] });
						stroke.Vertices.emplace_back(StrokeVertex{ .Position = outer, .Color = colors[i] });
						stroke.Vertices.emplace_back(StrokeVertex{ .Position = outerNext, .Color = colors[i] });
						stroke.Vertices.emplace_back(StrokeVertex{ .Position = currentPoint, .Color = colors[i] });

						stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 0));
						stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 1));
						stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 2));
						stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 2));
						stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 3));
						stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 0));

					}
				} else if (join == StrokeJoin::Round)
				{
					const size_t segmentCount = 12;
					const float startAngle = std::atan2(previousNormal.Y, previousNormal.X);
					const float endAngle = std::atan2(nextNormal.Y, nextNormal.X);

					float dir = sign(cross);
					if (dir == 0.0f) { dir = 1.0f; }

					float deltaAngle = endAngle - startAngle;
					while (deltaAngle > Math::PI) { deltaAngle -= Math::TAU; }
					while (deltaAngle < -Math::PI) { deltaAngle += Math::TAU; }

					if (dir > 0.0f and deltaAngle < 0.0f)
					{
						deltaAngle += Math::TAU;
					}
					else if (dir < 0.0f and deltaAngle > 0.0f)
					{
						deltaAngle -= Math::TAU;
					}

					float step = deltaAngle / static_cast<float>(segmentCount);
					uint32_t centerIndex = static_cast<uint32_t>(stroke.Vertices.size());
					stroke.Vertices.emplace_back(StrokeVertex{ .Position = currentPoint, .Color = colors[i] });

					float currentAngle = startAngle;
					for (size_t s = 0; s < segmentCount; ++s)
					{
						const float nextAngle = currentAngle + step;

						const Math::Float2 currentCirclePoint = currentPoint + Math::Float2(std::cos(currentAngle), std::sin(currentAngle)) * halfStrokeWeight;
						const Math::Float2 nextCirclePoint = currentPoint + Math::Float2(std::cos(nextAngle), std::sin(nextAngle)) * halfStrokeWeight;

						stroke.Vertices.emplace_back(StrokeVertex{ .Position = currentCirclePoint, .Color = colors[i] });
						stroke.Vertices.emplace_back(StrokeVertex{ .Position = nextCirclePoint, .Color = colors[i] });

						stroke.Indices.emplace_back(centerIndex);
						stroke.Indices.emplace_back(static_cast<uint32_t>(stroke.Vertices.size() - 2));
						stroke.Indices.emplace_back(static_cast<uint32_t>(stroke.Vertices.size() - 1));

						currentAngle = nextAngle;
					}
				}
			}

			// Handle end caps for open shapes
			if (not isClosed and (isStart and hasNextPoint) or (isEnd and hasPreviousPoint))
			{
				const Math::Float2 direction		= isStart ? nextDirection	: previousDirection;
				const Math::Float2 segmentNormal	= isStart ? nextNormal		: previousNormal;

				StrokeCapType strokeCapType = isStart ? strokeCap.Start : strokeCap.End;

				if (strokeCapType == StrokeCapType::Butt)
				{
					// Nothing to do for butt caps
				} else if (strokeCapType == StrokeCapType::Square)
				{
					const Math::Float2 capOffset = direction * (isStart ? halfStrokeWeight : -halfStrokeWeight);
					const Math::Float2 offsetLeft = currentPoint + segmentNormal * halfStrokeWeight;
					const Math::Float2 offsetRight = currentPoint - segmentNormal * halfStrokeWeight;

					const size_t baseIndex = stroke.Vertices.size();
					stroke.Vertices.emplace_back(StrokeVertex{ .Position = offsetLeft + capOffset, .Color = colors[i] });
					stroke.Vertices.emplace_back(StrokeVertex{ .Position = offsetRight + capOffset, .Color = colors[i] });
					stroke.Vertices.emplace_back(StrokeVertex{ .Position = offsetRight, .Color = colors[i] });
					stroke.Vertices.emplace_back(StrokeVertex{ .Position = offsetLeft, .Color = colors[i] });
					stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 0));
					stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 1));
					stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 2));
					stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 2));
					stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 3));
					stroke.Indices.emplace_back(static_cast<uint32_t>(baseIndex + 0));
				}
				else if (strokeCapType == StrokeCapType::Round)
				{
					constexpr size_t segmentCount = 12;
					const float startAngle = std::atan2(segmentNormal.Y, segmentNormal.X) + (isStart ? Math::PI : -Math::PI);
					const float endAngle = startAngle + (isStart ? Math::PI : -Math::PI);
					float deltaAngle = endAngle - startAngle;
					float step = deltaAngle / static_cast<float>(segmentCount);
					uint32_t centerIndex = static_cast<uint32_t>(stroke.Vertices.size());
					stroke.Vertices.emplace_back(StrokeVertex{ .Position = currentPoint, .Color = colors[i] });
					float currentAngle = startAngle;
					for (size_t s = 0; s < segmentCount; ++s)
					{
						const float nextAngle = currentAngle + step;

						const Math::Float2 currentCirclePoint = currentPoint + Math::Float2(std::cos(currentAngle), std::sin(currentAngle)) * halfStrokeWeight;
						const Math::Float2 nextCirclePoint = currentPoint + Math::Float2(std::cos(nextAngle), std::sin(nextAngle)) * halfStrokeWeight;

						stroke.Vertices.emplace_back(StrokeVertex{ .Position = currentCirclePoint, .Color = colors[i] });
						stroke.Vertices.emplace_back(StrokeVertex{ .Position = nextCirclePoint, .Color = colors[i] });
						stroke.Indices.emplace_back(centerIndex);
						stroke.Indices.emplace_back(static_cast<uint32_t>(stroke.Vertices.size() - 2));
						stroke.Indices.emplace_back(static_cast<uint32_t>(stroke.Vertices.size() - 1));

						currentAngle = nextAngle;
					}
				}
			}
		}

		return stroke;
	}
}