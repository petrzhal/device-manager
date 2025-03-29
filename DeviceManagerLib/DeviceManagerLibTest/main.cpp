#include "DeviceManagerLib.h"

#pragma warning(disable: 4251)

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <syncstream>
#include <print>
#include <chrono>

void printDeviceTree(const dm::DeviceTreeNode& node, int indent = 0)
{
	const std::string indentStr(indent, '\t');

	std::stringstream sstr;
	sstr << node.info << '\n';

	std::string buf;
	while(std::getline(sstr, buf))
	{
		std::println("{}{}", indentStr, buf);
	}

	for(const auto& child : node.children)
	{
		printDeviceTree(child, indent + 1);
	}
}

void monitorPerformance()
{
	dm::PerformanceMonitor perfMonitor;

	constexpr static auto tickDelay = std::chrono::seconds(1);

	while(true)
	{
		system("cls");

		const auto start = std::chrono::high_resolution_clock::now();

		std::println("{:^100}", "System Perfomance");
		std::println("CPU usage: {}%", perfMonitor.getCPUUsage());
		std::println("RAM usage: {}%", perfMonitor.getMemoryUsage());

		for(const auto& [disk, usagePercent] : perfMonitor.getDiskUsage())
		{
			std::println("Disk usage: {}\t{}%", disk, usagePercent);
		}

		std::println("GPU usage: {}%", perfMonitor.getGPUUsage());
		std::println("Network usage: {}B/sec", perfMonitor.getNetworkUsage());

		const auto diff = std::chrono::high_resolution_clock::now() - start;

		std::this_thread::sleep_for(tickDelay - diff);
	}
}

int main()
{
	setlocale(0, "");
	std::jthread perfThread(monitorPerformance);

	//dm::DeviceEnumerator enumerator;
	//std::vector<dm::DeviceInfo> devices;

	//auto callback = [&devices](const dm::DeviceInfo& device)
	//        {
	//            std::println("------------------------------------------");
	//            std::cout << device << '\n';

	//            devices.push_back(device);
	//        };

	//dm::DeviceTreeNode root = enumerator.getDeviceTree(callback);

	//std::println("\nRoot device name: ", root.info.name);
	//std::println("Device Tree:");
	//printDeviceTree(root);

	//vector<DeviceInfo> devices;

	//auto callback = [&devices](const DeviceInfo& device)
	//    {
	//        cout << "------------------------------------------\n";
	//        cout << "Name: " << device.name << "\n";
	//        cout << "Hardware ID: " << device.deviceId << "\n";
	//        cout << "Manufacturer: " << device.manufacturer << "\n";
	//        cout << "Driver version: " << device.driverVersion << "\n";
	//        cout << "Status: " << device.status << "\n";

	//        if (!device.additionalProperties.empty())
	//        {
	//            cout << "Additional properties:\n";

	//            for (const auto& prop : device.additionalProperties)
	//            {
	//                cout << "  " << prop.first << ": " << prop.second << "\n";
	//            }
	//        }

	//        devices.push_back(device);
	//    };

	////cout << "Finded devices: " << devices.size() << "\n";

	//DeviceTreeNode root = enumerator.getDeviceTree(callback);
	////DeviceTreeNode root;

	//cout << "\nRoot device name: " << root.info.name << "\n";
	//cout << "Device Tree:\n";
	//printDeviceTree(root);

	//std::jthread perfThread(monitorPerformance);

	//DeviceDiagnostic diagnostic;

	//if (!devices.empty())
	//{
	//    string devId = devices.front().deviceId;
	//    vector<string> dependents = diagnostic.getDependentDevices(devId);
	//    cout << "\nFor device " << devId << " related deviced found: " << dependents.size() << "\n";

	//    for (const auto& dep : dependents)
	//    {
	//        cout << "  " << dep << "\n";
	//    }
	//}
	//else
	//{
	//    cout << "\nNo devices found for dependency diagnostic.\n";
	//}

	//DeviceController controller;

	//if (!devices.empty())
	//{
	//    string devId = devices.front().deviceId;
	//    bool enabled = controller.enableDevice(devId);
	//    bool disabled = controller.disableDevice(devId);
	//    cout << "\nAttempt to enable device " << devId << " returned: " << boolalpha << enabled << "\n";
	//    cout << "Attempt to disavle device " << devId << " returned: " << boolalpha << disabled << "\n";
	//}
	//else
	//{
	//    cout << "\nNo devices for management\n";
	//}

	//return 0;
}