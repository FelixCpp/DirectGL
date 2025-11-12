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
	m_Spawner{ m_PositionProvider, 0.5f, 400.0f, },
	m_Weapon{ m_PositionProvider, 0.25f, 400.0f, },
	m_Font(DGL::Font::CreateFromFile("C:\\Windows\\Fonts\\Arial.ttf", 32))
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

void PlayingGameState::Update(const float deltaTime)
{
	m_Weapon.Shoot(m_Spawner.Enemies);

	m_Tower.Update(deltaTime);
	m_Spawner.Update(deltaTime);
	m_Weapon.Update(deltaTime);
}

void PlayingGameState::Show() const
{
	DGL::Background({ 0.1f, 0.1f, 0.1f, 1.0f });

	// Render the number of enemies currently on screen
	DGL::SetTextSize(32.0f);
	DGL::SetTextFont(m_Font.get());
	DGL::Text(std::format("Enemy Count: {}", m_Spawner.Enemies.size()), 20.0f, 50.0f);

	m_Tower.Show();
	m_Weapon.Show();
	m_Spawner.Show();
}