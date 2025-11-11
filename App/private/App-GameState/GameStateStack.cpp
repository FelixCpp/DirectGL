module;

#include <memory>

module App;

import :GameStateStack;

void GameStateStack::Push(std::unique_ptr<GameState> state)
{
	if (not m_Stack.empty())
	{
		m_Stack.top()->Suspend();
	}

	m_Stack.emplace(std::move(state))->Attach();
}

void GameStateStack::Pop()
{
	if (m_Stack.empty())
	{
		return;
	}

	m_Stack.top()->Detach();
	m_Stack.pop();

	if (not m_Stack.empty())
	{
		m_Stack.top()->Resume();
	}
}

GameState* GameStateStack::Peek()
{
	if (m_Stack.empty())
	{
		return nullptr;
	}

	return m_Stack.top().get();
}

void GameStateStack::Clear()
{
	while (not m_Stack.empty())
	{
		m_Stack.top()->Detach();
		m_Stack.pop();
	}
}