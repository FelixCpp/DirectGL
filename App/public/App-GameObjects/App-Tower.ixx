// Project Name : DirectGL
// File Name    : App-Tower.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

export module App:Tower;

import DirectGL;

import :PositionProvider;

class Tower
{
public:

	explicit Tower(const PositionProvider& positionProvider);

	void Update(float deltaTime);
	void Show() const;

	const PositionProvider*	PositionProvider;
	float					SpawnAnimationProgress;
};