#include <memory>
#include <vector>
#include <optional>
#include <unordered_map>

import DirectGL;

enum class Target
{
	Food,
	Poison
};



constexpr float EaseOutBounce(float t)
{
	if (t < 1.0f / 2.75f)
	{
		return 7.5625f * t * t;
	}
	else if (t < 2.0f / 2.75f)
	{
		t -= 1.5f / 2.75f;
		return 7.5625f * t * t + 0.75f;
	}
	else if (t < 2.5f / 2.75f)
	{
		t -= 2.25f / 2.75f;
		return 7.5625f * t * t + 0.9375f;
	}
	else
	{
		t -= 2.625f / 2.75f;
		return 7.5625f * t * t + 0.984375f;
	}
}

constexpr float EaseInOutBounce(float t)
{
	if (t < 0.5f)
	{
		return 0.5f * (1.0f - EaseOutBounce(1.0f - 2.0f * t));
	}
	else
	{
		return 0.5f * EaseOutBounce(2.0f * t - 1.0f) + 0.5f;
	}
}

float EaseInOutSine(float t)
{
	return -(std::cos(Math::PI * t) - 1.0f) / 2.0f;
}

struct Particle
{
	Math::Float2 Position;
	Math::Float2 Size;
	Target Type;

	float timeElapsed = 0.0f;
	float animationDuration = 1.0f;

	Particle(const Math::Float2 position, const Target type) :
		Position(position),
		Size(Math::Float2::Zero),
		Type(type)
	{
	}

	void Update(const float deltaTime)
	{
		timeElapsed += deltaTime;
	}

	void Show()
	{
		constexpr auto poisonColor = DGL::Color(255, 0, 0);
		constexpr auto foodColor = DGL::Color(0, 255, 0);

		const float progress = Math::Constrain(timeElapsed / animationDuration, 0.0f, 1.0f);
		const float easedProgress = EaseInOutBounce(progress);
		const float size = Math::Lerp(0.0f, 10.0f, easedProgress);

		auto targetColor = (Type == Target::Food) ? foodColor : poisonColor;
		targetColor.A = static_cast<uint8_t>(Math::Lerp(0.0f, 255.0f, easedProgress));

		DGL::Blend(DGL::BlendModes::Alpha);
		DGL::StrokeWeight(size);
		DGL::Stroke(targetColor);
		DGL::Point(Position.X, Position.Y);
	}
};


class Creature
{
	struct SearchResult
	{
		size_t Index;
		float DistanceSquared;
	};

	static constexpr float MinFoodFactor = -2.5f;
	static constexpr float MaxFoodFactor = 5.0f;
	static constexpr float MinPoisonFactor = -5.0f;
	static constexpr float MaxPoisonFactor = 2.50f;

public:

	explicit Creature(const float spawnX, const float spawnY) :
		m_Velocity(100.0f, 0.0f),
		m_Acceleration(0.0f, 0.0f),
		m_Position(spawnX, spawnY),
		m_MaxSpeed(100.0f),
		m_MaxForce(20.f),
		m_Size(16.0f),
		m_TimeAlive(0.0f),
		m_MaxLifetime(5.0f),
		m_AttractionFactors {
			{ Target::Food, Math::Random(MinFoodFactor, MaxFoodFactor) },
			{ Target::Poison, Math::Random(MinPoisonFactor, MaxPoisonFactor) }
		},
		m_PerceptionRadii {
			{ Target::Food, Math::Random(50.0f, 150.0f) },
			{ Target::Poison, Math::Random(50.0f, 150.0f) }
		}
	{
	}

	void Consume(std::vector<Particle>& food, const Target target)
	{
		const auto searchResult = GetNearestFood(food, target);
		if (not searchResult.has_value())
			return;

		// If the nearest food is close enough, eat it.
		if (searchResult->DistanceSquared < (m_Size * m_Size))
		{
			// The food is close enough, eat it.
			food.erase(food.begin() + searchResult->Index);

			// Decide whether to heal or damage the creature based on the target type.
			switch (target)
			{
				case Target::Food: m_MaxLifetime += 3.0f; break; //< Eating food increases the lifetime by one second.
				case Target::Poison: m_MaxLifetime -= 0.5f; break; //< Eating poison decreases the lifetime by half a second.
				default: break;
			}
		}
		else
		{
			// Get the steering force towards the food.
			const auto force = Seek(food[searchResult->Index].Position);

			DGL::StrokeWeight(3.0f);
			DGL::Stroke(DGL::Color(0, 255, 255));
			DGL::Line(m_Position.X, m_Position.Y, food[searchResult->Index].Position.X, food[searchResult->Index].Position.Y);

			// Apply the attraction factor to change the behavior of the creature.
			// A positive attraction factor makes the creature seek the food more aggressively,
			// while a negative attraction factor makes it avoid the food.
			const auto adjustedForce = force * m_AttractionFactors.at(target);

			// Apply the steering force to the creature.
			ApplyForce(adjustedForce);
		}
	}

	void ApplyForce(const Math::Float2& force)
	{
		m_Acceleration += force;
	}

	std::optional<SearchResult> GetNearestFood(const std::vector<Particle>& food, const Target target) const
	{
		// TODO(Felix): Consider using a quadtree for better performance with many food items.

		// Compute the nearest food item relative to the creatures position.
		float nearestDistance = std::numeric_limits<float>::max();
		size_t nearestIndex = std::numeric_limits<size_t>::max();

		// Get the perception radius for the current target type.
		const auto perceptionRadius = m_PerceptionRadii.at(target);

		for (size_t i = 0; i < food.size(); ++i)
		{
			const float distance = food[i].Position.DistanceSquared(m_Position);
			const bool isVisible = distance < (perceptionRadius * perceptionRadius);
			const bool beatsRecord = distance < nearestDistance;

			if (isVisible and beatsRecord)
			{
				nearestDistance = distance;
				nearestIndex = i;
			}
		}
		// Early out if there has been no nearest food found.
		if (nearestIndex == std::numeric_limits<size_t>::max())
			return std::nullopt;

		return SearchResult {
			.Index = nearestIndex,
			.DistanceSquared = nearestDistance
		};
	}

	Math::Float2 Seek(const Math::Float2& target) const
	{
		// Compute the desired velocity.
		// The desired velocity is a vector pointing from the current position to the target,
		// with a magnitude equal to the maximum speed.
		Math::Float2 desired = target - m_Position;
		desired = desired.Normalized() * m_MaxSpeed;

		// Compute the steering force.
		// The steering force is the difference between the desired velocity and the current velocity.
		Math::Float2 steer = desired - m_Velocity;

		// Limit the steering force to the maximum force.
		steer = steer.Limited(m_MaxForce);

		// Return the steering force.
		return steer;
	}

	void Update(const float deltaTime)
	{
		// Update the time alive
		m_TimeAlive += deltaTime;

		// Move the creature by its velocity
		m_Position += m_Velocity * deltaTime;
		m_Velocity += m_Acceleration * deltaTime;
		m_Acceleration = Math::Float2{ 0.0f, 0.0f };
	}

	void WrapAround(const Math::FloatBoundary& boundary)
	{
		if (m_Position.X < boundary.Left) m_Position.X = boundary.Right();
		if (m_Position.X > boundary.Right()) m_Position.X = boundary.Left;
		if (m_Position.Y < boundary.Top) m_Position.Y = boundary.Bottom();
		if (m_Position.Y > boundary.Bottom()) m_Position.Y = boundary.Top;
	}

	bool IsDead() const
	{
		return m_TimeAlive >= m_MaxLifetime;
	}

	bool IsClicked(const Math::Int2& mousePos) const
	{
		const float distanceSquared = m_Position.DistanceSquared(Math::Float2{ static_cast<float>(mousePos.X), static_cast<float>(mousePos.Y) });
		return distanceSquared < (m_Size * m_Size) * 5.0f;
	}

	void Show(const bool debug) const
	{
		constexpr DGL::Color healthyColor(0, 255, 0);
		constexpr DGL::Color dyingColor(255, 0, 0);

		// Get the direction vector of the creature based on its velocity.
		const auto direction = m_Velocity.Normalized();

		DGL::PushState();

		if (debug) {
			// Render perception radii

			//DGL::NoFill();
			DGL::Blend(DGL::BlendModes::Alpha);
			DGL::StrokeWeight(1.0f);

			// Render the food perception radius
			DGL::Stroke(DGL::Color(0, 255, 0));
			DGL::Fill(DGL::Color(0, 255, 0, 16));
			DGL::Circle(m_Position.X, m_Position.Y, m_PerceptionRadii.at(Target::Food) * 2.0f);

			// Render the poison perception radius
			DGL::Stroke(DGL::Color(255, 0, 0));
			DGL::Fill(DGL::Color(255, 0, 0, 16));
			DGL::Circle(m_Position.X, m_Position.Y, m_PerceptionRadii.at(Target::Poison) * 2.0f);
		}

		if (debug) {
			// Render the attraction factors

			DGL::StrokeWeight(2.0f);

			// Compute the direction vector
			const auto foodAttraction = Math::Remap(m_AttractionFactors.at(Target::Food), MinFoodFactor, MaxFoodFactor, -1.0f, 1.0f) * m_Size * 5.0f;
			const auto poisonAttraction = Math::Remap(m_AttractionFactors.at(Target::Poison), MinPoisonFactor, MaxPoisonFactor, -1.0f, 1.f) * m_Size * 5.0f;

			// Render the food attraction factor as a line
			DGL::Stroke(DGL::Color(0, 255, 0));
			DGL::Line(m_Position.X, m_Position.Y, m_Position.X + direction.X * foodAttraction, m_Position.Y + direction.Y * foodAttraction);

			// Render the poison attraction factor as a line
			DGL::Stroke(DGL::Color(255, 0, 0));
			DGL::Line(m_Position.X, m_Position.Y, m_Position.X + direction.X * poisonAttraction, m_Position.Y + direction.Y * poisonAttraction);
		}

		const float alpha = m_TimeAlive / m_MaxLifetime;
		const float clampedAlpha = Math::Constrain(1.0f - alpha, 0.0f, 1.0f);
		const DGL::Color color = dyingColor.Lerp(healthyColor, clampedAlpha);

		// Compute three points for the triangle based on the position and direction.
		const auto perp = direction.Perpendicular();
		const auto p1 = m_Position + direction * m_Size; // Tip of the triangle
		const auto p2 = m_Position - direction * m_Size * 0.5f + perp * m_Size * 0.5f; // Bottom left
		const auto p3 = m_Position - direction * m_Size * 0.5f - perp * m_Size * 0.5f; // Bottom right

		// Render the body
		DGL::Blend(DGL::BlendModes::Alpha);
		DGL::Fill(color);
		DGL::NoStroke();
		DGL::Triangle(p1.X, p1.Y, p2.X, p2.Y, p3.X, p3.Y);

		DGL::PopState();
	}

private:

	Math::Float2 m_Velocity;
	Math::Float2 m_Acceleration;
	Math::Float2 m_Position;

	float m_MaxSpeed;
	float m_MaxForce;
	float m_Size;
	float m_TimeAlive;
	float m_MaxLifetime;

	std::unordered_map<Target, float> m_AttractionFactors;
	std::unordered_map<Target, float> m_PerceptionRadii;

};

struct SpikesGame : DGL::Sketch
{
	std::vector<Particle> Food;
	std::vector<Particle> Poison;
	std::vector<Creature> Creatures;

	float m_TimeElapsedSinceLastSpawn = 0.0f;
	float m_SpawnInterval = 0.15f;

	bool IsDebugModeEnabled = false;

	bool Setup() override
	{
		DGL::SetWindowSize(1600, 900);
		
		const auto [width, height] = DGL::GetWindowSize();

		// Generate some random food and poison
		for (size_t i = 0; i < 100; i++)
		{
			Food.emplace_back(Math::Float2{ Math::Random(width), Math::Random(height) }, Target::Food);
		}

		for (size_t i = 0; i < 50; i++)
		{
			Poison.emplace_back(Math::Float2{ Math::Random(width), Math::Random(height) }, Target::Poison);
		}

		// Generate some creatures at random positions
		for (size_t i = 0; i < 10; ++i)
		{
			const float spawnX = Math::Random(width);
			const float spawnY = Math::Random(height);

			Creatures.emplace_back(spawnX, spawnY);
		}

		return true;
	}

	void Event(const System::WindowEvent& event) override
	{
		event.Visit(
			[this](const System::WindowEvent::KeyPressed& keyEvent)
			{
				if (keyEvent.Key == DGL::KeyboardKey::P)
					DGL::ToggleLoop();

				if (keyEvent.Key == DGL::KeyboardKey::D)
				{
					IsDebugModeEnabled = not IsDebugModeEnabled;
					DGL::Redraw();
				}
			},
			[](const auto&) {}
		);
	}

	void Draw() override
	{
		// Clear the background
		DGL::Background(DGL::Color(51, 51, 51, 150));

		const float gameSpeed = DGL::IsLooping() ? 1.0f / 60.0f : 0.0f;
		m_TimeElapsedSinceLastSpawn += gameSpeed;

		// Occasionally add new food and poison
		if (m_TimeElapsedSinceLastSpawn > m_SpawnInterval)
		{
			const auto [width, height] = DGL::GetWindowSize();
			Food.emplace_back(Math::Float2{ Math::Random(width), Math::Random(height) }, Target::Food);
			Poison.emplace_back(Math::Float2{ Math::Random(width), Math::Random(height) }, Target::Poison);
			m_TimeElapsedSinceLastSpawn = 0.0f;
		}

		// Update the creatures
		for (size_t i = 0; i < Creatures.size(); ++i)
		{
			Creatures[i].Consume(Food, Target::Food);
			Creatures[i].Consume(Poison, Target::Poison);
			Creatures[i].Update(gameSpeed);

			const auto [width, height] = DGL::GetWindowSize();
			Creatures[i].WrapAround(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)));
		}

		// Render the creatures
		for (size_t i = 0; i < Creatures.size(); ++i)
		{
			const bool isHovered = Creatures[i].IsClicked(DGL::GetMousePosition());
			Creatures[i].Show(isHovered or IsDebugModeEnabled);
		}

		// Render the food
		DGL::StrokeWeight(8.0f);
		for (size_t i = 0; i < Food.size(); ++i)
		{
			Food[i].Update(gameSpeed);
			Food[i].Show();
		}

		// Render the poison
		for (size_t i = 0; i < Poison.size(); ++i)
		{
			Poison[i].Update(gameSpeed);
			Poison[i].Show();
		}

		// Remove dead creatures
		std::erase_if(Creatures, [](const Creature& creature) { return creature.IsDead(); });

		// Restart the simulation if all creatures are dead
		if (Creatures.empty())
		{
			DGL::Restart();
		}
	}

	void Destroy() override
	{
	}
};

int main() {
	return DGL::Launch([] { return std::make_unique<SpikesGame>(); });
}
