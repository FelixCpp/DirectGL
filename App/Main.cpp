#include <memory>
#include <vector>

import DirectGL;

class Seeker
{
public:

	explicit Seeker(const float spawnX, const float spawnY):
		m_Position(spawnX, spawnY),
		m_Velocity(DGL::Float2::Random() * DGL::Random(100.0f)),
		m_Acceleration(DGL::Float2::Zero),
		m_Size(20.0f, 20.0f),
		m_MaxSpeed(DGL::Random(300.0f, 700.0f)),
		m_MaxForce(DGL::Random(800.0f, 1000.0f))
	{}

	void Seek(const DGL::Float2& target)
	{
		const DGL::Float2 steeringForce = GetSteeringForceTowards(target);
		ApplyForce(steeringForce);
	}

	void Update(const float deltaTime)
	{
		m_Position += m_Velocity * deltaTime;
		m_Velocity += m_Acceleration * deltaTime;
		m_Acceleration = DGL::Float2{ 0.0f, 0.0f };
	}

	void Show() const
	{
		DGL::PushTransform();
		{
			DGL::Translate(m_Position.X, m_Position.Y);
			DGL::Rotate(m_Velocity.Heading());
			DGL::Triangle(
				m_Size.X * 0.5f, 0.0f,
				-m_Size.X * 0.5f, m_Size.Y * 0.5f,
				-m_Size.X * 0.5f, -m_Size.Y * 0.5f
			);
		}
		DGL::PopTransform();
	}

private:

	inline void ApplyForce(const DGL::Float2& force) { m_Acceleration += force; }

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
		return true;
	}

	void Event(const System::WindowEvent& event) override
	{
		event.Visit([](const auto&) {});
	}

	void Draw(const float deltaTime) override
	{
		const auto mousePosition = DGL::Float2{ DGL::GetMousePosition() };

		for (Seeker& seeker : m_Seekers)
		{
			seeker.Seek(mousePosition);
			seeker.Update(deltaTime);
		}

		DGL::Background(DGL::Color(51, 51, 51));

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

};

int main() {
	return DGL::Launch([] { return std::make_unique<DirectGLGame>(); });
}
