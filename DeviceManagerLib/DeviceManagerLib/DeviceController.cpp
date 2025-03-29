#include "pch.h"
#include "DeviceController.h"

namespace dm
{
	bool DeviceController::enableDevice(const std::string& deviceId) const
	{
		HDEVINFO hDevInfo = NULL;
		SP_DEVINFO_DATA devInfoData;

		if(!FindDeviceById(deviceId, hDevInfo, devInfoData))
		{
			std::cerr << "Device with ID " << deviceId << " not found." << std::endl;

			return false;
		}

		bool result = ChangeDeviceState(hDevInfo, devInfoData, DICS_ENABLE);
		SetupDiDestroyDeviceInfoList(hDevInfo);

		return result;
	}

	bool DeviceController::disableDevice(const std::string& deviceId) const
	{
		HDEVINFO hDevInfo = NULL;
		SP_DEVINFO_DATA devInfoData;

		if(!FindDeviceById(deviceId, hDevInfo, devInfoData))
		{
			std::cerr << "Device with ID " << deviceId << " not found." << std::endl;

			return false;
		}

		bool result = ChangeDeviceState(hDevInfo, devInfoData, DICS_DISABLE);
		SetupDiDestroyDeviceInfoList(hDevInfo);

		return result;
	}

	bool DeviceController::updateDriver(const std::string& deviceId, const std::string& driverPackagePath) const
	{
		BOOL rebootRequired = FALSE;
		BOOL res = UpdateDriverForPlugAndPlayDevicesA(NULL, deviceId.c_str(), driverPackagePath.c_str(), 0, &rebootRequired);

		if(!res)
		{
			std::cerr << "UpdateDriverForPlugAndPlayDevices failed for device " << deviceId << std::endl;
		}

		return res;
	}

	bool DeviceController::restoreDeviceSettings(const std::string& deviceId) const
	{
		return reinitializeDevice(deviceId);
	}

	bool DeviceController::reinitializeDevice(const std::string& deviceId) const
	{
		if(!disableDevice(deviceId))
		{
			std::cerr << "Failed to disable device " << deviceId << std::endl;

			return false;
		}

		Sleep(1000);

		if(!enableDevice(deviceId))
		{
			std::cerr << "Failed to enable device " << deviceId << std::endl;

			return false;
		}

		return true;
	}

	bool DeviceController::FindDeviceById(const std::string& instanceId, HDEVINFO& hDevInfo, SP_DEVINFO_DATA& devInfoData)
	{
		hDevInfo = SetupDiGetClassDevsA(nullptr, nullptr, nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT);

		if(hDevInfo == INVALID_HANDLE_VALUE)
		{
			std::cerr << "SetupDiGetClassDevsA failed." << std::endl;

			return false;
		}

		char buffer[512] = { 0 };
		devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

		for(DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++)
		{
			if(SetupDiGetDeviceInstanceIdA(hDevInfo, &devInfoData, buffer, sizeof(buffer), NULL) && instanceId == std::string(buffer))
			{
				return true;
			}
		}

		SetupDiDestroyDeviceInfoList(hDevInfo);

		return false;
	}

	bool DeviceController::ChangeDeviceState(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData, DWORD stateChange)
	{
		SP_PROPCHANGE_PARAMS propChangeParams;
		memset(&propChangeParams, 0, sizeof(propChangeParams));
		propChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
		propChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
		propChangeParams.StateChange = stateChange;
		propChangeParams.Scope = DICS_FLAG_GLOBAL;
		propChangeParams.HwProfile = 0;

		if(!SetupDiSetClassInstallParamsA(hDevInfo, &devInfoData, &propChangeParams.ClassInstallHeader, sizeof(propChangeParams)))
		{
			std::cerr << "SetupDiSetClassInstallParamsA failed." << std::endl;

			return false;
		}

		if(!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &devInfoData))
		{
			std::cerr << "SetupDiCallClassInstaller failed." << std::endl;

			return false;
		}

		return true;
	}
} // namespace dm