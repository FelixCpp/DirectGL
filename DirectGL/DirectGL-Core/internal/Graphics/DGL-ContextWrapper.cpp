module;

#include <format>

module DirectGL;

namespace DGL
{
	ContextWrapper::ContextWrapper(const WindowProvider& windowProvider) :
		m_WindowProvider(windowProvider)
	{
	}

	Startup::StartupTask::Continuation ContextWrapper::Setup()
	{
		m_Context = System::CreateContext({
			.MajorVersion = 4,
			.MinorVersion = 6,
			.IsDebuggingContext = true,
			.IsCompatibilityContext = false,
			.ParentWindow = m_WindowProvider()->GetNativeHandle(),
			.OnError = [this](std::string_view message)
			{
				Error(std::format("OpenGL context error: {}", message));
			}
		});

		if (m_Context == nullptr)
		{
			Error("Couldn't create OpenGL rendering context.");
			return Abort;
		}

		SetVerticalSyncEnabled(true);

		Info("Successfully created OpenGL rendering context.");
		return Continue;
	}

	void ContextWrapper::Teardown()
	{
		Info("Tearing down OpenGL rendering context.");
		m_Context.reset();
	}

	void ContextWrapper::SetVerticalSyncEnabled(const bool enabled)
	{
		Debug(std::format("Setting vertical sync to {}", enabled));
		m_Context->SetVerticalSyncEnabled(enabled);
	}

	void ContextWrapper::Flush()
	{
		m_Context->Flush();
	}
}