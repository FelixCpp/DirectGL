module;

#include <format>
#include <cmath>


module App;

import DirectGL;

import :PlayingGameState;

std::unique_ptr<DGL::Font> font;

PlayingGameState::PlayingGameState() :
	m_PositionProvider{ DGL::GetViewport().Center() },
	m_Tower{ m_PositionProvider },
	m_Spawner{ m_PositionProvider, 0.25f, 400.0f, },
	m_Weapon{ m_PositionProvider, 0.2f, 400.0f, }
{
	font = DGL::Font::CreateFromFile("C:\\Windows\\Fonts\\ARIAL.ttf", 130);
}

void PlayingGameState::Event(const DGL::WindowEvent& event)
{
	event.Visit(
		[this](const DGL::WindowEvent::Resized& resized)
		{
			m_PositionProvider.SetPosition(DGL::GetViewport().Center());
		},
		[](const auto&) {}
	);
}
float scaleX = 1.0f;
float scaleY = 0.5f;
float t = 0.0f;

void PlayingGameState::Update(float deltaTime)
{
	if (DGL::IsKeyDown(DGL::Input::KeyboardKey::Space))
	{
		deltaTime *= 10.0f;
	}

	t += deltaTime;
	scaleX = 200.0f + 100.0f * std::sin(t);
	scaleY = 200.0f + 100.0f * std::cos(t);


//	m_Tower.Update(deltaTime);
//	m_Spawner.Update(deltaTime);
//	m_Weapon.Update(deltaTime);
//	m_Weapon.Shoot(m_Spawner.Enemies);
}

struct Quad
{
	DGL::Math::FloatBoundary Boundary;
	DGL::Math::Float4 Color;
};

void PlayingGameState::Show() const
{
	DGL::Background({ 0.1f, 0.1f, 0.1f, 1.0f });

	float cx = 800.0f;
	float cy = 540.0f;
	float rx = scaleX;
	float ry = scaleY;

	float left = cx - rx;
	float top = cy - ry;
	float right = cx + rx;
	float bottom = cy + ry;

	float rounding = DGL::GetMousePosition().X / DGL::GetViewport().Width * 300.0f;
	DGL::Warning(std::format("Rounding: {}", rounding));
	DGL::SetRectMode(DGL::RectModeLTRB());
	DGL::SetStrokeWeight(25.0f);
	DGL::SetStrokeColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	DGL::SetFillColor({ 0.0f, 0.0f, 1.0f, 1.0f });
	DGL::RoundedRect(200, 200, DGL::GetMousePosition().X, DGL::GetMousePosition().Y, DGL::Math::BorderRadius::All(50.0f));

	//DGL::SetStrokeColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	//DGL::SetFillColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	//DGL::SetStrokeWeight(40.0f);
	//DGL::Ellipse(cx, cy, rx, ry);

	//static std::vector<Quad> quads = []()
	//{
	//	std::vector<Quad> quads(10'000);
	//	for (size_t i = 0; i < quads.size(); ++i)
	//	{
	//		const auto width = DGL::GetViewport().Width;
	//		const auto height = DGL::GetViewport().Height;

	//		const float x = DGL::Math::Random(0.0f, width);
	//		const float y = DGL::Math::Random(0.0f, height);
	//		const float size = DGL::Math::Random(5.0f, 20.0f);

	//		const DGL::Math::Float4 color = {
	//			DGL::Math::Random(0.0f, 1.0f),
	//			DGL::Math::Random(0.0f, 1.0f),
	//			DGL::Math::Random(0.0f, 1.0f),
	//			DGL::Math::Random(0.0f, 1.0f),
	//		};

	//		quads[i] = Quad{ DGL::Math::FloatBoundary::FromLTWH(x, y, size, size), color };
	//	}

	//	return quads;
	//}();

	//// Generate 10'000 random quads on screen
	//// using the DGL::Math::Random function
	//DGL::SetBlendMode(DGL::BlendMode::Additive);
	//for (size_t i = 0; i < quads.size(); ++i)
	//{
	//	const Quad& q = quads[i];

	//	DGL::SetFillColor(q.Color);
	//	DGL::Ellipse(q.Boundary.Left, q.Boundary.Top, q.Boundary.Width, q.Boundary.Height);
	//}

//	m_Tower.Show();
//	m_Weapon.Show();
//	m_Spawner.Show();
}