module;

module DirectGL;

import DirectGL.DPI;
import DirectGL.Logging;

namespace DGL
{
	StartupTask::Continuation ConfigureDPIStartupTask::Setup()
	{
		if (EnableDPIAwareness())
		{
			return Continue;
		}

		Warning("Couldn't set DPI Awareness");
		return Continue; //!< DPI awareness couldn't be set, but we can continue anyway
	}

	void ConfigureDPIStartupTask::Teardown()
	{
	}
}