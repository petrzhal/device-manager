#include "pch.h"
#include "DeviceDiagnostic.h"

namespace dm
{
	std::vector<std::string> DeviceDiagnostic::getDependentDevices(const std::string& deviceId) const
	{
		std::vector<std::string> dependentDevices;
		DEVINST devInst = 0;
		CONFIGRET cr = CM_Locate_DevNodeA(&devInst, const_cast<char*>(deviceId.c_str()), CM_LOCATE_DEVNODE_NORMAL);

		if(cr != CR_SUCCESS)
		{
			std::cerr << "CM_Locate_DevNodeA failed for deviceId: " << deviceId << std::endl;

			return dependentDevices;
		}

		DEVINST child;
		cr = CM_Get_Child(&child, devInst, 0);

		if(cr != CR_SUCCESS)
		{
			return dependentDevices;
		}

		const ULONG bufSize = MAX_DEVICE_ID_LEN;
		char buffer[MAX_DEVICE_ID_LEN] = { 0 };

		do
		{
			cr = CM_Get_Device_IDA(child, buffer, bufSize, 0);

			if(cr == CR_SUCCESS)
			{
				dependentDevices.push_back(std::string(buffer));
			}
			else
			{
				std::cerr << "CM_Get_Device_IDA failed for a child device." << std::endl;
			}

			DEVINST sibling;
			cr = CM_Get_Sibling(&sibling, child, 0);

			if(cr == CR_SUCCESS)
			{
				child = sibling;
			}
			else
			{
				break;
			}
		} while(true);

		return dependentDevices;
	}
} // namespace dm