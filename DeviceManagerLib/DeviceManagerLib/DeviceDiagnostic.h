#pragma once

#include "DeviceManagerLibGlobal.h"

namespace dm
{

class DEVICE_MANAGER_LIB_EXPORT DeviceDiagnostic {
public:
    DeviceDiagnostic() = default;
    ~DeviceDiagnostic() = default;

    std::vector<std::string> getDependentDevices(const std::string &deviceId) const;
};

} // namespace dm


