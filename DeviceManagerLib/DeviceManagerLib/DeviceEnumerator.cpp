#include "pch.h"
#include "DeviceEnumerator.h"
#include "DeviceDiagnostic.h"

namespace dm
{

DeviceEnumerator::DeviceEnumerator() = default;

DeviceEnumerator::~DeviceEnumerator() = default;


std::vector<DeviceInfo> DeviceEnumerator::enumerateDevices(const DeviceCallback& callback) const
{
#ifdef _WIN32
    std::vector<DeviceInfo> devices;

    HDEVINFO hDevInfo = SetupDiGetClassDevsA(nullptr, nullptr, nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES);

    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        std::cerr << "SetupDiGetClassDevsA failed." << std::endl;

        return devices;
    }

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD i = 0;

    while (SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData))
    {
        DeviceInfo info;
        info.deviceId = getDeviceProperty(hDevInfo, devInfoData, SPDRP_HARDWAREID);
        info.name = getDeviceProperty(hDevInfo, devInfoData, SPDRP_DEVICEDESC);
        info.manufacturer = getDeviceProperty(hDevInfo, devInfoData, SPDRP_MFG);

        DWORD drvVersion = GetDriverVersion(hDevInfo, devInfoData);

        if (drvVersion != 0)
        {
            std::string sVersion = FormatDriverVersion(drvVersion);
            info.driverVersion = sVersion;
        }
        else
        {
            info.driverVersion = "Unknown";
        }

        DWORD installState = 0;
        DWORD bufferSize = sizeof(installState);

        if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_INSTALL_STATE, NULL,
              reinterpret_cast<PBYTE>(&installState),
              bufferSize, &bufferSize))
        {
            if (installState == 0)
                info.status = "Installed";
            else
                info.status = "Not installed";
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
#else
    return std::vector<DeviceInfo>();
#endif
}


#ifdef _WIN32
std::string DeviceEnumerator::getDeviceProperty(HDEVINFO hDevInfo, SP_DEVINFO_DATA &devInfoData, DWORD property) const
{
    DWORD dataType = 0, bufferSize = 0;

    SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, property, &dataType, nullptr, 0, &bufferSize);

    if (bufferSize == 0)
    {
        return "";
    }

    std::vector<char> buffer(bufferSize);

    if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, property, &dataType,
        reinterpret_cast<PBYTE>(buffer.data()), bufferSize, nullptr))
    {
        return std::string(buffer.data());
    }

    return "";
}
#endif


void addDependentDevices(DeviceTreeNode& node,
    const DeviceDiagnostic& diagnostic,
    std::set<std::string>& trackedIds)
{
    if (trackedIds.find(node.info.deviceId) != trackedIds.end())
        return;

    trackedIds.insert(node.info.deviceId);

    std::vector<std::string> depIds = diagnostic.getDependentDevices(node.info.deviceId);
    for (const auto& depId : depIds)
    {
        if (trackedIds.find(depId) != trackedIds.end())
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

    std::map<std::string, DeviceTreeNode> groups;
    for (const auto& dev : devices)
    {
        std::string key = dev.deviceClass.empty() ? "Unknown" : dev.deviceClass;
        if (groups.find(key) == groups.end())
        {
            DeviceTreeNode node;
            node.info.name = key;
            node.info.deviceId = key; 
            groups[key] = node;
        }

        DeviceTreeNode child;
        child.info = dev;
        groups[key].children.push_back(child);
    }

    for (const auto& group : groups)
    {
        root.children.push_back(group.second);
    }

    DeviceDiagnostic diagnostic;

    std::set<std::string> visited;

    std::function<void(DeviceTreeNode&)> addDeps = [&](DeviceTreeNode& node)
        {
            addDependentDevices(node, diagnostic, visited);
            for (auto& child : node.children)
            {
                addDeps(child);
            }
        };
    addDeps(root);

    return root;
}


DWORD DeviceEnumerator::GetDriverVersion(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData) const
{
    if (!SetupDiBuildDriverInfoList(hDevInfo, &devInfoData, SPDIT_CLASSDRIVER))
    {
        return 0;
    }
    SP_DRVINFO_DATA drvInfo;
    ZeroMemory(&drvInfo, sizeof(SP_DRVINFO_DATA));
    drvInfo.cbSize = sizeof(SP_DRVINFO_DATA);

    if (!SetupDiGetSelectedDriver(hDevInfo, &devInfoData, &drvInfo))
    {
        SetupDiDestroyDriverInfoList(hDevInfo, &devInfoData, SPDIT_CLASSDRIVER);
        return 0;
    }

    DWORD dwVersion = drvInfo.DriverVersion;
    SetupDiDestroyDriverInfoList(hDevInfo, &devInfoData, SPDIT_CLASSDRIVER);

    return dwVersion;
}


std::string DeviceEnumerator::FormatDriverVersion(DWORD dwVersion) const
{
    unsigned int major = HIWORD(dwVersion);
    unsigned int minor = LOWORD(dwVersion);

    if (major == 0 && minor == 0)
        return "Unknown";

    char buff[16];
    sprintf_s(buff, "%d.%d", major, minor);
    return buff;
}

} // namespace dm
