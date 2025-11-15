module;

#include <memory>
#include <vector>


#include <format>

module App;

import DirectGL;

import :PlayingGameState;

PlayingGameState::PlayingGameState() :
	m_PositionProvider{ DGL::GetViewport().Center() },
	m_Tower{ m_PositionProvider },
	m_Spawner{ m_PositionProvider, 0.25f, 400.0f, },
	m_Weapon{ m_PositionProvider, 0.2f, 400.0f, },
	m_Font(DGL::Font::CreateFromFile("C:\\Windows\\Fonts\\arial.ttf", 120))
{}

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
float f;

void PlayingGameState::Update(float deltaTime)
{
	f += deltaTime * 13.0f;

	if (DGL::IsKeyDown(DGL::Input::KeyboardKey::Space))
	{
		deltaTime *= 10.0f;
	}

	m_Tower.Update(deltaTime);
	m_Spawner.Update(deltaTime);
	m_Weapon.Update(deltaTime);
	m_Weapon.Shoot(m_Spawner.Enemies);
}

void PlayingGameState::Show() const
{
	DGL::Background({ 0.1f, 0.1f, 0.1f, 1.0f });

	const auto [mx, my] = (DGL::Math::Float2)DGL::GetMousePosition() * 0.0f + DGL::Math::Float2{ 200.0f, 200.0f };

	// Render the number of enemies currently on screen
	//DGL::SetTextSize(120.0f);
	//DGL::SetTextFont(m_Font.get());
	//DGL::SetTextAlign(DGL::TextAlignment::TopLeft);
	//DGL::Text("Hello, World!", mx, my);
	//
	//DGL::SetStrokeWeight(4.0f);
	//DGL::SetStrokeColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	//DGL::Point(mx, my);
	
	m_Tower.Show();
	m_Weapon.Show();
	m_Spawner.Show();
}