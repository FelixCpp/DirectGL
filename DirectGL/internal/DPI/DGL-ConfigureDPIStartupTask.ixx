// Project Name : DGL
// File Name    : DGL-ConfigureDPIStartupTask.ixx
// Author       : Felix Busch
// Created Date : 2025/09/18

export module DGL:ConfigureDPIStartupTask;

import Startup;

namespace DGL
{
	struct ConfigureDPIStartupTask : Startup::StartupTask
	{
		Continuation Setup() override;
		void Teardown() override;
	};
}