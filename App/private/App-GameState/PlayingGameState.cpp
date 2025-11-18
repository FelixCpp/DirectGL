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

void PlayingGameState::Update(float deltaTime)
{
//	m_Tower.Update(deltaTime);
//	m_Spawner.Update(deltaTime);
//	m_Weapon.Update(deltaTime);
//	m_Weapon.Shoot(m_Spawner.Enemies);
}

void PlayingGameState::Show() const
{
	DGL::Background({ 0.1f, 0.1f, 0.1f, 1.0f });

	constexpr float cx = 800.0f;
	constexpr float cy = 540.0f;
	const float r = DGL::GetMousePosition().X / DGL::GetViewport().Width * 255.0f;
	const float g = DGL::GetMousePosition().Y;// / DGL::GetViewport().Height * 455.0f;
	
	DGL::SetRectMode(DGL::RectModeLTRB());
	DGL::SetStrokeWeight(55.0f);
	DGL::SetStrokeColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	DGL::SetFillColor({ 1.4f, 0.4f, 0.4f, 1.0f });
	DGL::RoundedRect(300.0f, 300.0f, r, g, DGL::Math::BorderRadius::All(r));

	DGL::SetEllipseMode(DGL::EllipseModeLTRB());
	DGL::SetStrokeWeight(25.0f);
	DGL::SetStrokeColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	DGL::SetFillColor({ 0.0f, 0.0f, 1.0f, 1.0f });
	//DGL::Ellipse(cx, cy, DGL::GetMousePosition().X, DGL::GetMousePosition().Y);

//	m_Tower.Show();
//	m_Weapon.Show();
//	m_Spawner.Show();
}