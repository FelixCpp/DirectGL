// Project Name : DirectGL
// File Name    : App.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

export module App;

import :GameStateStack;

struct AppData
{
	GameStateStack GameStates;
};

module :private;

constexpr DGL::Math::Float3 COLOR_BLUE = { 0.1f, 0.5f, 0.8f };
constexpr DGL::Math::Float3 COLOR_RED = { 0.8f, 0.3f, 0.2f };
constexpr DGL::Math::Float3 COLOR_GOLDEN = { 1.0f, 0.84f, 0.0f };
constexpr DGL::Math::Float3 COLOR_WHITE = { 1.0f, 1.0f, 1.0f };

AppData g_AppData = {};