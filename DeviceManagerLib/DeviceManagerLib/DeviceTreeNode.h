#pragma once

#include "DeviceManagerLibGlobal.h"
#include "DeviceInfo.h"

namespace dm
{

struct DEVICE_MANAGER_LIB_EXPORT DeviceTreeNode {
    DeviceInfo info;
    std::vector<DeviceTreeNode> children;
};

} // namespace dm

