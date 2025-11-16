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

void PlayingGameState::Update(float deltaTime)
{
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
	DGL::RoundedRect(100.0f, 100.0f, 300.0f, 300.0f, DGL::Math::BorderRadius::Only(DGL::Math::BorderRadiusOnly {
		.TopLeft = DGL::Math::Radius::Elliptical(50.0f, 20.0f),
		.TopRight = DGL::Math::Radius::Elliptical(10.0f, 20.0f),
		.BottomRight = DGL::Math::Radius::Elliptical(20.0f, 20.0f),
		.BottomLeft = DGL::Math::Radius::Elliptical(10.0f, 50.0f),
	}));

	m_Tower.Show();
	m_Weapon.Show();
	m_Spawner.Show();
}