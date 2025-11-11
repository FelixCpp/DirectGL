// Project Name : DirectGL
// File Name    : GameState.hpp
// Author       : Felix Busch
// Created Date : 2025/11/10

export module App:GameState;

import DirectGL;

struct GameState
{
	virtual ~GameState() = default;

	virtual void Attach() {}
	virtual void Detach() {}

	virtual void Resume() {}
	virtual void Suspend() {}

	virtual void Event(const DGL::WindowEvent& event) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Show() const = 0;
};