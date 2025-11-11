// Project Name : DirectGL
// File Name    : App-PositionProvider.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

export module App:PositionProvider;

import DirectGL;

struct PositionProvider
{
	virtual ~PositionProvider() = default;
	virtual const DGL::Math::Float2& GetPosition() const = 0;
};

class StaticPositionProvider : public PositionProvider
{
public:

	explicit StaticPositionProvider(const DGL::Math::Float2& position) :
		m_Position{ position }
	{}

	const DGL::Math::Float2& GetPosition() const override
	{
		return m_Position;
	}

private:

	DGL::Math::Float2 m_Position;

};

class DynamicPositionProvider : public PositionProvider
{
public:

	explicit DynamicPositionProvider(const DGL::Math::Float2& position) :
		m_Position{ position }
	{}

	void SetPosition(const DGL::Math::Float2& position)
	{
		m_Position = position;
	}

	const DGL::Math::Float2& GetPosition() const override
	{
		return m_Position;
	}

private:

	DGL::Math::Float2 m_Position;

};