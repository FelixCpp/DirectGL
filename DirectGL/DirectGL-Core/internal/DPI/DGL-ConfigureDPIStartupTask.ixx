// Project Name : DGL
// File Name    : DGL-ConfigureDPIStartupTask.ixx
// Author       : Felix Busch
// Created Date : 2025/09/18

export module DirectGL:ConfigureDPIStartupTask;

import DirectGL.Startup;

namespace DGL
{
	struct ConfigureDPIStartupTask : StartupTask
	{
		Continuation Setup() override;
		void Teardown() override;
	};
}