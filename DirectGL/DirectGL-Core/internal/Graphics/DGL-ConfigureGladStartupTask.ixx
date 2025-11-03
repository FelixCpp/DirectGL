// Project Name : DirectGL
// File Name    : DGL-ConfigureGladStartupTask.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

export module DirectGL:ConfigureGladStartupTask;

import DirectGL.Startup;

namespace DGL
{
	struct ConfigureGladStartupTask : StartupTask
	{
		Continuation Setup() override;
		void Teardown() override;
	};
}