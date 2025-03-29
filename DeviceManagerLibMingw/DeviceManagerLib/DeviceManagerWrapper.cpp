#include "DeviceManagerLib.h"

using namespace dm;

extern "C" {

    DEVICE_MANAGER_LIB_EXPORT DeviceEnumerator* CreateDeviceEnumerator() {
        return new DeviceEnumerator();
    }

    DEVICE_MANAGER_LIB_EXPORT dm::DeviceController* CreateDeviceController() {
        return new dm::DeviceController();
    }

    DEVICE_MANAGER_LIB_EXPORT dm::DeviceDiagnostic* CreateDeviceDiagnostic() {
        return new dm::DeviceDiagnostic();
    }

    DEVICE_MANAGER_LIB_EXPORT dm::PerformanceMonitor* CreatePerformanceMonitor() {
        return new dm::PerformanceMonitor();
    }

    DEVICE_MANAGER_LIB_EXPORT dm::DeviceInfo* CreateDeviceInfo() {
        return new dm::DeviceInfo();
    }

    DEVICE_MANAGER_LIB_EXPORT dm::DeviceTreeNode* CreateDeviceTreeNode() {
        return new dm::DeviceTreeNode();
    }
}