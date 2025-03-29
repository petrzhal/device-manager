#pragma once

#include "DeviceManagerLibGlobal.h"

namespace dm
{

class DEVICE_MANAGER_LIB_EXPORT PerformanceMonitor
{
    using DisksType = std::pair<std::string, float>;
    using DisksStorage = std::map<std::string, float>;
public:
    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;

    double getCPUUsage() const;
    double getMemoryUsage() const;
    DisksStorage getDiskUsage() const;
    double getGPUUsage() const;
    double getNetworkUsage() const;
};

} // namespace dm

