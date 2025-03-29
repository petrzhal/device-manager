#pragma once

#include "DeviceManagerLibGlobal.h"
#include "DeviceInfo.h"
#include "DeviceTreeNode.h"

namespace dm {
    class DEVICE_MANAGER_LIB_EXPORT DeviceEnumerator {
        using DeviceCallback = std::function<void(const DeviceInfo&)>;

    public:
        DeviceEnumerator() = default;
        ~DeviceEnumerator() = default;

        [[nodiscard]] std::vector<DeviceInfo> enumerateDevices(const DeviceCallback& callback) const;

        [[nodiscard]] DeviceTreeNode getDeviceTree(const DeviceCallback& deviceCallback = nullptr) const;

    private:
        std::string getDeviceProperty(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData, DWORD property) const;
        DWORD GetDriverVersion(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData) const;
        std::string FormatDriverVersion(DWORD dwVersion) const;
    };

} // namespace dm

