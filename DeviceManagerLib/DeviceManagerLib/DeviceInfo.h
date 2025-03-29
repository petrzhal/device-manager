#pragma once

#include "DeviceManagerLibGlobal.h"

namespace dm
{

struct DEVICE_MANAGER_LIB_EXPORT DeviceInfo
{
    std::string deviceId;
    std::string name;
    std::string manufacturer;
    std::string deviceClass;
    std::string status;
    std::unordered_map<std::string, std::string> additionalProperties;

    friend std::ostream& operator<<(std::ostream& ostr, const DeviceInfo& obj) {
        ostr << std::format("Device Name: {}\n", obj.name)
            << std::format("Device ID: {}\n", obj.deviceId)
            << std::format("Manufacturer: {}\n", obj.manufacturer)
            << std::format("Status: {}\n", obj.status)
            << std::format("Type: {}\n", obj.deviceClass);

        if(!obj.additionalProperties.empty()) {
            ostr << "Additional properties:\n";

            for(const auto& prop : obj.additionalProperties) {
                ostr << std::format("{}\n", prop);
            }
        }

        return ostr;
    }
};

} // namespace dm

