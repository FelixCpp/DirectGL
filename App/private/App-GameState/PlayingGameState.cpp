module App;

import DirectGL;

import :PlayingGameState;

std::unique_ptr<DGL::Font> font;
std::unique_ptr<DGL::Image2D> image;
std::unique_ptr<DGL::ImageSampler> sampler;

PlayingGameState::PlayingGameState() :
	m_PositionProvider{ DGL::GetViewport().Center() },
	m_Tower{ m_PositionProvider },
	m_Spawner{ m_PositionProvider, 0.25f, 400.0f, },
	m_Weapon{ m_PositionProvider, 0.2f, 400.0f, }
{
	font = DGL::Font::CreateFromFile("C:\\Windows\\Fonts\\ARIAL.ttf", 42);
	image = DGL::Image2D::CreateFromFile("Squares.png");
	sampler = DGL::ImageSampler::Create(DGL::ImageSamplerFilterMode::Nearest);
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
	DGL::SetTextFont(font.get());
	DGL::Text("abcdefghijklmnopqrstuvwxyzABCDEFHIJKLMNOPQRSTUVWXYZ", 100.0f, 100.0f);

	uint8_t data[4 * 3 * 3] = {
		255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255,
		255, 0, 255, 255, 0, 255, 255, 255, 255, 0, 255, 255,
		255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255,
	};
	image->Update(8, 1, 3, 3, data);

	DGL::SetImageSampler(sampler.get());
	//DGL::Image(*image, 200.0f, 200.0f, 400.0f, 400.0f);

	m_Tower.Show();
	m_Weapon.Show();
	m_Spawner.Show();
}