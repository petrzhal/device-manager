#pragma once

#include "DeviceManagerLibGlobal.h"
#include <iostream>
#include <string>
#include <unordered_map>

namespace dm
{
    struct DeviceInfo
    {
        std::string deviceId;
        std::string name;
        std::string manufacturer;
        std::string deviceClass;
        std::string status;
        std::unordered_map<std::string, std::string> additionalProperties;

        friend std::ostream& operator<<(std::ostream& ostr, const DeviceInfo& obj)
        {
            ostr << "Device Name: " << obj.name << "\n"
                 << "Device ID: " << obj.deviceId << "\n"
                 << "Manufacturer: " << obj.manufacturer << "\n"
                 << "Status: " << obj.status << "\n"
                 << "Type: " << obj.deviceClass << "\n";

            if (!obj.additionalProperties.empty())
            {
                ostr << "Additional properties:\n";
                for (const auto& prop : obj.additionalProperties)
                {
                    ostr << prop.first << ": " << prop.second << "\n";
                }
            }
            return ostr;
        }
    };
} // namespace dm
