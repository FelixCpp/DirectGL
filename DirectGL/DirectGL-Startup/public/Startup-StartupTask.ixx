// Project Name : DirectGL-Startup
// File Name    : Startup-StartupTask.ixx
// Author       : Felix Busch
// Created Date : 2025/09/17

export module DirectGL.Startup:StartupTask;

export namespace DGL
{
	struct StartupTask
	{
		enum Continuation
		{
			Continue,
			Abort,
		};

		virtual ~StartupTask() = default;
		virtual Continuation Setup() = 0;
		virtual void Teardown() = 0;
	};
}