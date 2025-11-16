// Project Name : DirectGL
// File Name    : PlayingGameState.hpp
// Author       : Felix Busch
// Created Date : 2025/11/10

export module App:PlayingGameState;

import :GameState;
import :Tower;
import :EnemySpawner;
import :BulletWeapon;

class PlayingGameState : public GameState
{
public:

	PlayingGameState();

	void Event(const DGL::WindowEvent& event) override;
	void Update(float deltaTime) override;
	void Show() const override;

private:

	DynamicPositionProvider m_PositionProvider;

	Tower m_Tower;
	EnemySpawner m_Spawner;
	BulletWeapon m_Weapon;

};