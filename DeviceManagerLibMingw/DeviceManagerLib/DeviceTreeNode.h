#pragma once

#include "DeviceManagerLibGlobal.h"
#include "DeviceInfo.h"

namespace dm
{
    struct DeviceTreeNode
	{
		DeviceInfo info;
		std::vector<DeviceTreeNode> children;
	};
} // namespace dm
