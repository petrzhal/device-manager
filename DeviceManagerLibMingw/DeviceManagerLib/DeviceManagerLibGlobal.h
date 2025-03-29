#pragma once

#ifdef DEVICE_MANAGER_LIB_LIBRARY
	#define DEVICE_MANAGER_LIB_EXPORT __declspec(dllexport)
#else
	#define DEVICE_MANAGER_LIB_EXPORT __declspec(dllimport)
#endif

namespace dm
{
    class DeviceEnumerator;
    class DeviceController;
    class DeviceDiagnostic;
    class PerformanceMonitor;
    class DeviceInfo;
    class DeviceTreeNode;

    typedef DeviceEnumerator* (*CreateDeviceEnumeratorFunc)();
    typedef DeviceController* (*CreateDeviceControllerFunc)();
    typedef DeviceDiagnostic* (*CreateDeviceDiagnosticFunc)();
    typedef PerformanceMonitor* (*CreatePerformanceMonitorFunc)();
    typedef DeviceInfo* (*CreateDeviceInfoFunc)();
    typedef DeviceTreeNode* (*CreateDeviceTreeNodeFunc)();

} // namespace dm
