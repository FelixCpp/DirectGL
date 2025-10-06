// Project Name : LaunchScope
// File Name    : StartupTask.ixx
// Author       : Felix Busch
// Created Date : 2025/09/17

export module Startup:StartupTask;

export namespace Startup
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