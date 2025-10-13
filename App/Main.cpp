#include <memory>
#include <vector>
#include <optional>

import DirectGL;

class Consumable
{
public:

	explicit Consumable(const DGL::Float2 position, const DGL::Color color, const float radius):
		m_Position(position),
		m_Color(color),
		m_Radius(radius),
		m_ScaleFactor(0.0f),
		m_ScaleSpeed(DGL::Random(0.15f, 1.0f))
	{
	}

	DGL::Float2 GetPosition() const
	{
		return m_Position;
	}

	float GetRadius() const
	{
		return m_Radius;
	}

	void Update(const float deltaTime)
	{
		m_ScaleFactor = DGL::Min(m_ScaleFactor + deltaTime * m_ScaleSpeed, 1.0f);
	}

	void Show() const
	{
		const float scale = DGL::EaseInSine(m_ScaleFactor);

		DGL::Stroke(m_Color);
		DGL::Fill(m_Color.WithAlpha(128));
		DGL::SetEllipseMode(DGL::EllipseModeCenterRadius());
		DGL::Ellipse(m_Position.X, m_Position.Y, m_Radius * scale, m_Radius * scale);
	}

private:

	DGL::Float2 m_Position;
	DGL::Color m_Color;
	float m_Radius;
	float m_ScaleFactor;
	float m_ScaleSpeed;

};

class Seeker
{
public:

	explicit Seeker(const float spawnX, const float spawnY):
		m_Position(spawnX, spawnY),
		m_Velocity(DGL::Float2::Random() * DGL::Random(100.0f)),
		m_Acceleration(DGL::Float2::Zero),
		m_Size(20.0f, 20.0f),
		m_MaxSpeed(200.0f),
		m_MaxForce(520.0f)
	{}

	void Consume(const std::vector<Consumable>& consumables, const float attractionFactor, auto consume)
	{
		// Get information about the nearest consumable in order to retrieve it later
		const auto nearest = GetNearestConsumable(consumables);
		if (nearest == std::nullopt)
		{
			return;
		}

		// Get the nearest consumable using the information retrieved
		const Consumable* nearestConsumable = &consumables[nearest->ElementIndex];

		// Check if the seeker is close enough to consume the consumable
		const float consumeDistance = nearestConsumable->GetPosition().Distance(m_Position);
		if (Hits(*nearestConsumable))
		{
			consume(nearest->ElementIndex);
			return;
		}

		// Compute the steering force towards the nearest consumable
		Seek(nearestConsumable->GetPosition(), attractionFactor);
	}

	void Update(const float deltaTime)
	{
		m_Position += m_Velocity * deltaTime;
		m_Velocity += m_Acceleration * deltaTime;
		m_Acceleration = DGL::Float2{ 0.0f, 0.0f };
	}

	void Show() const
	{
		// Compute a color which is based on the speed of the seeker
		const float speedRatio = m_Velocity.Length() / m_MaxSpeed;
		const DGL::Color color = DGL::Color::FromHSL(speedRatio * 120.0f, 1.0f, 0.5f);

		DGL::PushTransform();
		{
			DGL::Translate(m_Position.X, m_Position.Y);
			DGL::Rotate(m_Velocity.Heading());
			DGL::NoStroke();
			DGL::Fill(color);
			DGL::Triangle(
				m_Size.X * 0.5f, 0.0f,
				-m_Size.X * 0.5f, m_Size.Y * 0.5f,
				-m_Size.X * 0.5f, -m_Size.Y * 0.5f
			);
		}
		DGL::PopTransform();
	}

private:

	void Seek(const DGL::Float2& target, const float attractionFactor)
	{
		// Get the steering force towards the target and apply it with an attraction factor
		const DGL::Float2 steeringForce = GetSteeringForceTowards(target) * attractionFactor;

		// Apply the steering force to the acceleration
		m_Acceleration += steeringForce;
	}

	[[nodiscard]] DGL::Float2 GetSteeringForceTowards(const DGL::Float2& point) const
	{
		// Compute the desired velocity to reach the target point
		const DGL::Float2 desiredVelocity = (point - m_Position).Normalized() * m_MaxSpeed;

		// Compute the steering force needed to adjust the current velocity towards the desired velocity
		DGL::Float2 steeringForce = desiredVelocity - m_Velocity;

		// Limit the steering force to the maximum allowable force
		steeringForce = steeringForce.Limited(m_MaxForce);

		return steeringForce;
	}

	bool Hits(const Consumable& consumable) const
	{
		const float distance = consumable.GetPosition().Distance(m_Position);
		return distance < (m_Size.X + m_Size.Y) * 0.25f + consumable.GetRadius();
	}

private:

	struct NearestConsumable
	{
		size_t ElementIndex;
		float DistanceSquared;
	};

	[[nodiscard]] std::optional<NearestConsumable> GetNearestConsumable(const std::vector<Consumable>& consumables) const
	{
		float nearestDistanceSquared = std::numeric_limits<float>::max();
		size_t elementIndex = std::numeric_limits<size_t>::max();

		for (size_t i = 0; i < consumables.size(); ++i)
		{
			const float distanceSquared = (consumables[i].GetPosition() - m_Position).LengthSquared();
			if (distanceSquared < nearestDistanceSquared)
			{
				nearestDistanceSquared = distanceSquared;
				elementIndex = i;
			}
		}

		if (elementIndex == std::numeric_limits<size_t>::max())
		{
			return std::nullopt;
		}

		return NearestConsumable {
			elementIndex,
			nearestDistanceSquared
		};
	}

	DGL::Float2 m_Position;
	DGL::Float2 m_Velocity;
	DGL::Float2 m_Acceleration;
	DGL::Float2 m_Size;

	float m_MaxSpeed;
	float m_MaxForce;

};

class DirectGLGame : public DGL::Sketch
{
public:

	bool Setup() override
	{
		DGL::SetWindowSize(1600, 900);

		m_Seekers.emplace_back(800.0f, 450.0f);
		m_Seekers.emplace_back(400.0f, 450.0f);

		for (int i = 0; i < 50; ++i)
		{
			const DGL::Float2 position = { DGL::Random(0.0f, 1600.0f), DGL::Random(0.0f, 900.0f) };
			const DGL::Color color = DGL::Color::FromHSL(DGL::Random(0.0f, 360.0f), 1.0f, 0.5f);
			const float radius = DGL::Random(5.0f, 20.0f);

			m_Consumables.emplace_back(position, color, radius);
		}

		return true;
	}

	void Event(const System::WindowEvent& event) override
	{
		event.Visit([](const auto&) {});
	}

	void Draw(const float deltaTime) override
	{
		for (Consumable& consumable : m_Consumables)
		{
			consumable.Update(deltaTime);
		}

		for (Seeker& seeker : m_Seekers)
		{
			seeker.Consume(m_Consumables, 1.0f, [this](size_t elementIndex)
			{
				m_Consumables.erase(m_Consumables.begin() + elementIndex);
			});

			seeker.Update(deltaTime);
		}

		DGL::Background(DGL::Color(51, 51, 51));

		for (const Consumable& consumable : m_Consumables)
		{
			consumable.Show();
		}

		for (const Seeker& seeker : m_Seekers)
		{
			seeker.Show();
		}
	}

	void Destroy() override
	{
	}

private:

	std::vector<Seeker> m_Seekers;
	std::vector<Consumable> m_Consumables;

};

int main() {
	return DGL::Launch([] { return std::make_unique<DirectGLGame>(); });
}
