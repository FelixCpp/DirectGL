// Project Name : DGL
// File Name    : DGL-WindowRapper.ixx
// Author       : Felix Busch
// Created Date : 2025/09/17

module;

#include <memory>
#include <optional>
#include <string_view>
#include <string>

export module DirectGL:WindowStartupTask;

import Startup;
import System.Monitor;
import System.Window;

namespace DGL
{
	class WindowWrapper : public System::Window, public Startup::StartupTask
	{
	public:

		explicit WindowWrapper(std::shared_ptr<System::MonitorProvider> monitorProvider);

		Continuation Setup() override;
		void Teardown() override;

		std::optional<System::WindowEvent> PollEvent() override;
		void SetPosition(const Math::Int2& position) override;
		Math::Int2 GetPosition() const override;
		void SetSize(const Math::Uint2& size) override;
		Math::Uint2 GetSize() const override;
		void SetTitle(std::string_view title) override;
		std::string GetTitle() const override;
		void SetVisible(bool visibility) override;
		bool IsVisible() const override;
		void SetResizable(bool resizability) override;
		bool IsResizable() const override;
		bool RequestFocus() const override;
		System::NativeWindowHandle GetNativeHandle() const override;

	private:

		std::unique_ptr<Window> m_Window;
		std::shared_ptr<System::MonitorProvider> m_MonitorProvider;

	};
}