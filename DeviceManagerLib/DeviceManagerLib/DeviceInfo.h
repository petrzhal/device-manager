#pragma once

#include "DeviceManagerLibGlobal.h"

namespace dm
{

struct DEVICE_MANAGER_LIB_EXPORT DeviceInfo
{
    std::string deviceId;
    std::string name;
    std::string manufacturer;
    std::string driverVersion;
    std::string deviceClass;
    std::string status;
    std::map<std::string, std::string> additionalProperties;
};

} // namespace dm

