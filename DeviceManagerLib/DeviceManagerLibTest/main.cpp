#include "DeviceManagerLib.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <syncstream>

using namespace std;
using namespace dm;


void printDeviceTree(const DeviceTreeNode& node, int indent = 0)
{
    string indentStr(indent, ' ');

    cout << indentStr << "Device Name: " << node.info.name << "\n";
    cout << indentStr << "Device ID  : " << node.info.deviceId << "\n";
    cout << indentStr << "Manufacturer: " << node.info.manufacturer << "\n";
    cout << indentStr << "Driver Version: " << node.info.driverVersion << "\n";
    cout << indentStr << "Status: " << node.info.status << "\n";
    cout << indentStr << "Type: " << node.info.deviceClass << "\n";

    if (!node.info.additionalProperties.empty()) 
    {
        cout << indentStr << "Additional Properties:\n";

        for (const auto& prop : node.info.additionalProperties) 
        {
            cout << indentStr << "  " << prop.first << ": " << prop.second << "\n";
        }
    }

    cout << "\n";

    for (const auto& child : node.children) 
    {
        printDeviceTree(child, indent + 2);
    }
}


void monitorPerformance() {
    PerformanceMonitor perfMonitor;
    while (true) 
    {
        system("cls");

        cout << "Системная производительность:\n";
        cout << "CPU usage: " << perfMonitor.getCPUUsage() << "%\n";
        cout << "Memory usage: " << perfMonitor.getMemoryUsage() << "%\n";

        for (const auto& [disk, usagePercent] : perfMonitor.getDiskUsage())
        {
            cout << "Disk usage: " << disk << " " << usagePercent << "%\n";
        }
        
        cout << "GPU usage: " << perfMonitor.getGPUUsage() << "%\n";
        cout << "Network usage: " << perfMonitor.getNetworkUsage() << " B/sec\n";

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


int main()
{
    setlocale(0, "");

    DeviceEnumerator enumerator;

    vector<DeviceInfo> devices;

    auto callback = [&devices](const DeviceInfo& device)
            {
                cout << "------------------------------------------\n";
                cout << "Name: " << device.name << "\n";
                cout << "Hardware ID: " << device.deviceId << "\n";
                cout << "Manufacturer: " << device.manufacturer << "\n";
                cout << "Driver version: " << device.driverVersion << "\n";
                cout << "Status: " << device.status << "\n";
                cout << "Type: " << device.deviceClass << "\n";

                if (!device.additionalProperties.empty())
                {
                    cout << "Additional properties:\n";

                    for (const auto& prop : device.additionalProperties)
                    {
                        cout << "  " << prop.first << ": " << prop.second << "\n";
                    }
                }

                devices.push_back(device);
            };

    DeviceTreeNode root = enumerator.getDeviceTree(callback);
    //DeviceTreeNode root;

    cout << "\nRoot device name: " << root.info.name << "\n";
    cout << "Device Tree:\n";
    printDeviceTree(root);


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