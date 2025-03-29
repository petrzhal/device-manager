#pragma once

#include "DeviceManagerLibGlobal.h"

namespace dm
{
    class DeviceDiagnostic
	{
	public:
		DeviceDiagnostic() = default;
		~DeviceDiagnostic() = default;

		std::vector<std::string> getDependentDevices(const std::string& deviceId) const;
	};
} // namespace dm
