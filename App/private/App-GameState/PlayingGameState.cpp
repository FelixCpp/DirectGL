module App;

import DirectGL;

import :PlayingGameState;

PlayingGameState::PlayingGameState() :
	m_PositionProvider{ DGL::GetViewport().Center() },
	m_Tower{ m_PositionProvider },
	m_Spawner{ m_PositionProvider, 0.25f, 400.0f, },
	m_Weapon{ m_PositionProvider, 0.2f, 400.0f, }
{
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

void PlayingGameState::Update(const float deltaTime)
{
	m_Tower.Update(deltaTime);
	m_Spawner.Update(deltaTime);
	m_Weapon.Update(deltaTime);
	m_Weapon.Shoot(m_Spawner.Enemies);
}

void PlayingGameState::Show() const
{
	DGL::Background({ 0.1f, 0.1f, 0.1f, 1.0f });

	m_Tower.Show();
	m_Weapon.Show();
	m_Spawner.Show();
}