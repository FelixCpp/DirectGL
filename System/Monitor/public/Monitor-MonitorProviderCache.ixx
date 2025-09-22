// Project Name : Monitor
// File Name    : Monitor-MonitorProviderCache.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <memory>

export module System.Monitor:MonitorProviderCache;

import :MonitorProvider;

export namespace System
{
	class MonitorProviderCache : public MonitorProvider
	{
	public:

		explicit MonitorProviderCache(std::shared_ptr<MonitorProvider> provider);

		[[nodiscard]] std::optional<Monitor> GetPrimaryMonitor() const override;
		[[nodiscard]] std::vector<Monitor> GetAvailableMonitors() const override;

		void Clear();
		void Reset(std::shared_ptr<MonitorProvider> provider);

	private:

		std::shared_ptr<MonitorProvider> m_MonitorProvider;
		mutable std::optional<Monitor> m_PrimaryMonitor;
		mutable std::vector<Monitor> m_AvailableMonitors;

	};
}