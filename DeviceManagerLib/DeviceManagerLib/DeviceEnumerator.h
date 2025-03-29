#pragma once

#include "DeviceManagerLibGlobal.h"
#include "DeviceInfo.h"
#include "DeviceTreeNode.h"

namespace dm
{

class DEVICE_MANAGER_LIB_EXPORT DeviceEnumerator {
	using DeviceCallback = std::function<void(const DeviceInfo&)>;

public:
   DeviceEnumerator();
   ~DeviceEnumerator();

   std::vector<DeviceInfo> enumerateDevices(const DeviceCallback& callback) const;

   DeviceTreeNode getDeviceTree(const DeviceCallback& deviceCallback = nullptr) const;

private:
#ifdef _WIN32
   std::string getDeviceProperty(HDEVINFO hDevInfo, SP_DEVINFO_DATA &devInfoData, DWORD property) const;
   DWORD GetDriverVersion(HDEVINFO hDevInfo, SP_DEVINFO_DATA &devInfoData) const;
   std::string FormatDriverVersion(DWORD dwVersion) const;
#endif
};

} // namespace dm

