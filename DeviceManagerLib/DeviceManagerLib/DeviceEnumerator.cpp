#include "pch.h"
#include "DeviceEnumerator.h"
#include "DeviceDiagnostic.h"

namespace dm
{
	std::vector<DeviceInfo> DeviceEnumerator::enumerateDevices(const DeviceCallback& callback) const
	{
		std::vector<DeviceInfo> devices;

		HDEVINFO hDevInfo = SetupDiGetClassDevsA(nullptr, nullptr, nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES);

		if(hDevInfo == INVALID_HANDLE_VALUE)
		{
			std::cerr << "SetupDiGetClassDevsA failed." << std::endl;

			return devices;
		}

		SP_DEVINFO_DATA devInfoData{};
		devInfoData.cbSize = sizeof(devInfoData);
		DWORD i = 0;

		while(SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData))
		{
			DeviceInfo info;
			info.deviceId = getDeviceProperty(hDevInfo, devInfoData, SPDRP_HARDWAREID);
			info.name = getDeviceProperty(hDevInfo, devInfoData, SPDRP_DEVICEDESC);
			info.manufacturer = getDeviceProperty(hDevInfo, devInfoData, SPDRP_MFG);

			info.deviceClass = getDeviceProperty(hDevInfo, devInfoData, SPDRP_CLASS);

			DWORD installState = 0;
			DWORD bufferSize = sizeof(installState);

			if(SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_INSTALL_STATE, NULL,
				reinterpret_cast<PBYTE>(&installState),
				bufferSize, &bufferSize))
			{
				info.status = installState == 0 ? "Installed" : "Not installed";
			}
			else
			{
				info.status = "Unknown";
			}

			callback(info);
			devices.push_back(info);
			++i;
		}

		SetupDiDestroyDeviceInfoList(hDevInfo);

		return devices;
	}

	std::string DeviceEnumerator::getDeviceProperty(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData, DWORD property) const
	{
		DWORD dataType = 0, bufferSize = 0;

		SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, property, &dataType, nullptr, 0, &bufferSize);

		if(bufferSize == 0)
		{
			return "";
		}

		std::vector<char> buffer(bufferSize);

		if(SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, property, &dataType,
			reinterpret_cast<PBYTE>(buffer.data()), bufferSize, nullptr))
		{
			return std::string(buffer.data());
		}

		return "";
	}

	void addDependentDevices(DeviceTreeNode& node,
		const DeviceDiagnostic& diagnostic,
		std::set<std::string>& trackedIds)
	{
		if(trackedIds.find(node.info.deviceId) != trackedIds.end())
			return;

		trackedIds.insert(node.info.deviceId);

		std::vector<std::string> depIds = diagnostic.getDependentDevices(node.info.deviceId);
		for(const auto& depId : depIds)
		{
			if(trackedIds.find(depId) != trackedIds.end())
				continue;

			DeviceTreeNode depNode;
			depNode.info.deviceId = depId;
			depNode.info.name = depId;
			node.children.push_back(depNode);

			addDependentDevices(node.children.back(), diagnostic, trackedIds);
		}
	}

	DeviceTreeNode DeviceEnumerator::getDeviceTree(const DeviceCallback& deviceCallback) const
	{
		DeviceTreeNode root;
		root.info.name = "All devices";
		root.info.deviceId = "ROOT";
		root.info.manufacturer = "";
		root.info.deviceClass = "All";

		std::vector<DeviceInfo> devices = enumerateDevices(deviceCallback);

		std::unordered_map<std::string, DeviceTreeNode> groups;

		for(const auto& dev : devices)
		{
			const std::string key = dev.deviceClass.empty() ? "Unknown" : dev.deviceClass;

			if(groups.contains(key))
			{
				DeviceTreeNode node;
				node.info.name = key;
				node.info.deviceId = key;

				groups[key] = std::move(node);
			}

			DeviceTreeNode child{ .info = dev };
			groups[key].children.emplace_back(child);
		}

		for(const auto& group : groups | std::views::values)
		{
			root.children.push_back(group);
		}

		DeviceDiagnostic diagnostic;
		std::set<std::string> visited;

		std::function<void(DeviceTreeNode&)> addDeps = [&](DeviceTreeNode& node) {
			addDependentDevices(node, diagnostic, visited);
			std::ranges::for_each(node.children, addDeps);
		};
		addDeps(root);

		return root;
	}
} // namespace dm