// Project Name : DirectGL
// File Name    : GameStateStack.hpp
// Author       : Felix Busch
// Created Date : 2025/11/10

module;

#include <memory>
#include <stack>

export module App:GameStateStack;

import :GameState;

class GameStateStack
{
public:

	GameStateStack() = default;

	void Push(std::unique_ptr<GameState> state);
	void Pop();
	GameState* Peek();

	void Clear();

private:

	std::stack<std::unique_ptr<GameState>> m_Stack;

};