#pragma once

#include "DeviceManagerLibGlobal.h"

namespace dm
{
	class DEVICE_MANAGER_LIB_EXPORT DeviceController
	{
	public:
		DeviceController() = default;
		~DeviceController() = default;

		bool enableDevice(const std::string& deviceId) const;
		bool disableDevice(const std::string& deviceId) const;
		bool updateDriver(const std::string& deviceId, const std::string& driverPackagePath) const;
		bool restoreDeviceSettings(const std::string& deviceId) const;
		bool reinitializeDevice(const std::string& deviceId) const;

	private:
		static bool FindDeviceById(const std::string& instanceId, HDEVINFO& hDevInfo, SP_DEVINFO_DATA& devInfoData);
		static bool ChangeDeviceState(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData, DWORD stateChange);
	};
} // namespace dm
