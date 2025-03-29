#pragma once

#include "DeviceManagerLibGlobal.h"

namespace dm
{

class PerformanceMonitor
{
public:
    using DisksType = std::pair<std::string, float>;
    using DisksStorage = std::map<std::string, float>;

    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;

    double getCPUUsage() const;
    double getMemoryUsage() const;
    DisksStorage getDiskUsage() const;
    double getGPUUsage() const;
    int getNetworkUsage() const;
};

} // namespace dm

