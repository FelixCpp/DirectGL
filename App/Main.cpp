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

class Creature
{
	struct SearchResult
	{
		size_t Index;
		float DistanceSquared;
	};

	static constexpr float MinFactor = -10.0f;
	static constexpr float MaxFactor = 10.0f;

public:

	explicit Creature(const float spawnX, const float spawnY) :
		m_Velocity(100.0f, 0.0f),
		m_Acceleration(0.0f, 0.0f),
		m_Position(spawnX, spawnY),
		m_MaxSpeed(50.0f),
		m_MaxForce(10.f),
		m_Size(16.0f),
		m_TimeAlive(0.0f),
		m_MaxLifetime(5.0f),
		m_AttractionFactors {
			{ Target::Food, Math::Random(MinFactor, MaxFactor) },
			{ Target::Poison, Math::Random(MinFactor, MaxFactor) }
		},
		m_PerceptionRadii {
			{ Target::Food, Math::Random(50.0f, 150.0f) },
			{ Target::Poison, Math::Random(50.0f, 150.0f) }
		}
	{
	}

	void Consume(std::vector<Math::Float2>& food, const Target target)
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
				case Target::Food: m_MaxLifetime += 1.0f; break; //< Eating food increases the lifetime by one second.
				case Target::Poison: m_MaxLifetime -= 0.5f; break; //< Eating poison decreases the lifetime by half a second.
				default: break;
			}
		}
		else
		{
			// Get the steering force towards the food.
			const auto force = Seek(food[searchResult->Index]);

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

	std::optional<SearchResult> GetNearestFood(const std::vector<Math::Float2>& food, const Target target) const
	{
		// TODO(Felix): Consider using a quadtree for better performance with many food items.

		// Compute the nearest food item relative to the creatures position.
		float nearestDistance = std::numeric_limits<float>::max();
		size_t nearestIndex = std::numeric_limits<size_t>::max();

		// Get the perception radius for the current target type.
		const auto perceptionRadius = m_PerceptionRadii.at(target);

		for (size_t i = 0; i < food.size(); ++i)
		{
			const float distance = food[i].DistanceSquared(m_Position);
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

	void Show() const
	{
		constexpr DGL::Color healthyColor(0, 255, 0);
		constexpr DGL::Color dyingColor(255, 0, 0);

		// Get the direction vector of the creature based on its velocity.
		const auto direction = m_Velocity.Normalized();

		DGL::PushState();

		{
			// Render perception radii

			//DGL::NoFill();
			DGL::Blend(DGL::BlendModes::Burn);
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

		{
			// Render the attraction factors

			DGL::StrokeWeight(2.0f);

			// Compute the direction vector
			const auto foodAttraction = Math::Remap(m_AttractionFactors.at(Target::Food), MinFactor, MaxFactor, -1.0f, 1.0f) * m_Size * 5.0f;
			const auto poisonAttraction = Math::Remap(m_AttractionFactors.at(Target::Poison), MinFactor, MaxFactor, -1.0f, 1.f) * m_Size * 5.0f;

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
	std::vector<Math::Float2> Food;
	std::vector<Math::Float2> Poison;
	std::vector<Creature> Creatures;

	bool Setup() override
	{
		DGL::SetWindowSize(1600, 900);

		const auto [width, height] = DGL::GetWindowSize();

		// Generate some random food and poison
		for (size_t i = 0; i < 40; i++)
		{
			Food.emplace_back(Math::Float2{ Math::Random(width), Math::Random(height) });
		}

		for (size_t i = 0; i < 20; i++)
		{
			Poison.emplace_back(Math::Float2{ Math::Random(width), Math::Random(height) });
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
	}

	void Draw() override
	{
		// Occasionally add new food and poison
		if (Math::Random(1.0f) < 0.1f)
		{
			const auto [width, height] = DGL::GetWindowSize();
			Food.emplace_back(Math::Float2{ Math::Random(width), Math::Random(height) });
			Poison.emplace_back(Math::Float2{ Math::Random(width), Math::Random(height) });
		}

		// Update the creatures
		for (size_t i = 0; i < Creatures.size(); ++i)
		{
			Creatures[i].Consume(Food, Target::Food);
			Creatures[i].Consume(Poison, Target::Poison);
			Creatures[i].Update(1.0f / 60.0f);

			const auto [width, height] = DGL::GetWindowSize();
			Creatures[i].WrapAround(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)));
		}

		// Clear the background
		DGL::Background(DGL::Color(51, 51, 51));

		// Render the food
		for (size_t i = 0; i < Food.size(); ++i)
		{
			DGL::NoStroke();
			DGL::Fill(DGL::Color(0, 255, 0));
			DGL::Ellipse(Food[i].X, Food[i].Y, 8.0f, 8.0f);
		}

		// Render the poison
		for (size_t i = 0; i < Poison.size(); ++i)
		{
			DGL::NoStroke();
			DGL::Fill(DGL::Color(255, 0, 0));
			DGL::Ellipse(Poison[i].X, Poison[i].Y, 8.0f, 8.0f);
		}

		// Render the creatures
		for (size_t i = 0; i < Creatures.size(); ++i)
		{
			Creatures[i].Show();
		}

		// Remove dead creatures
		std::erase_if(Creatures, [](const Creature& creature) { return creature.IsDead(); });
	}

	bool IsDone() const
	{
		return Creatures.empty();
	}

	void Destroy() override
	{
	}
};

struct InfiniteRepeatingSketch : DGL::Sketch
{
	bool Setup() override
	{
		Game = std::make_unique<SpikesGame>();
		return Game->Setup();
	}

	void Event(const System::WindowEvent& event) override
	{
		if (Game) Game->Event(event);
	}

	void Draw() override
	{
		if (Game == nullptr)
		{
			Game = std::make_unique<SpikesGame>();
			Game->Setup();
		}

		Game->Draw();

		if (Game->IsDone())
		{
			Game->Destroy();
			Game.reset();
		}
	}

	void Destroy() override
	{
		if (Game) Game->Destroy();
	}

	std::unique_ptr<SpikesGame> Game;
};

int main() {
	return DGL::Launch([] { return std::make_unique<InfiniteRepeatingSketch>(); });
}
