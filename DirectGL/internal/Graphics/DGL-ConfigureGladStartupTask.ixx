// Project Name : DirectGL
// File Name    : DGL-ConfigureGladStartupTask.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

export module DGL:ConfigureGladStartupTask;

import Startup;

namespace DGL
{
	struct ConfigureGladStartupTask : public Startup::StartupTask
	{
		Continuation Setup() override;
		void Teardown() override;
	};
}