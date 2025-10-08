// Project Name : DirectGL-Logging
// File Name    : Logging.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <memory>

export module DirectGL.Logging;

export import :AsyncLogger;
export import :GlobalLogger;

module :private;

std::weak_ptr<LogForge::Logger> g_RootLogger;