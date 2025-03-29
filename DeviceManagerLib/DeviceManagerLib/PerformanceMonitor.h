#pragma once

#include "DeviceManagerLibGlobal.h"

namespace dm
{

class DEVICE_MANAGER_LIB_EXPORT PerformanceMonitor
{
public:
    PerformanceMonitor();
    ~PerformanceMonitor();

    double getCPUUsage() const;
    double getMemoryUsage() const;
    std::map<std::string, double> getDiskUsage() const;
    double getGPUUsage() const;
    int getNetworkUsage() const;
};

} // namespace dm

