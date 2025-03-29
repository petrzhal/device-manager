#include "pch.h"
#include "PerformanceMonitor.h"

namespace dm {

PerformanceMonitor::PerformanceMonitor() = default;

PerformanceMonitor::~PerformanceMonitor() = default;


double PerformanceMonitor::getCPUUsage() const
{
    PDH_HQUERY cpuQuery;
    PDH_HCOUNTER cpuTotal;
    PDH_FMT_COUNTERVALUE counterVal;

    PDH_STATUS status = PdhOpenQueryA(NULL, 0, &cpuQuery);

    if (status != ERROR_SUCCESS)
    {
        std::cerr << "PdhOpenQueryA failed. Error code: " << status << std::endl;

        return -1.0;
    }

    status = PdhAddCounterA(cpuQuery, "\\���������(_Total)\\% ������������� ����������", 0, &cpuTotal);

    if (status != ERROR_SUCCESS)
    {
        std::cerr << "PdhAddCounterA failed. Error code: " << status << std::endl;
        PdhCloseQuery(cpuQuery);

        return -1.0;
    }

    status = PdhCollectQueryData(cpuQuery);

    if (status != ERROR_SUCCESS)
    {
        std::cerr << "PdhCollectQueryData (first sample) failed. Error code: " << status << std::endl;
        PdhCloseQuery(cpuQuery);

        return -1.0;
    }

    Sleep(1000);

    status = PdhCollectQueryData(cpuQuery);

    if (status != ERROR_SUCCESS)
    {
        std::cerr << "PdhCollectQueryData (second sample) failed. Error code: " << status << std::endl;
        PdhCloseQuery(cpuQuery);

        return -1.0;
    }

    status = PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);

    if (status != ERROR_SUCCESS)
    {
        std::cerr << "PdhGetFormattedCounterValue failed. Error code: " << status << std::endl;
        PdhCloseQuery(cpuQuery);

        return -1.0;
    }

    PdhCloseQuery(cpuQuery);

    return counterVal.doubleValue;
}



double PerformanceMonitor::getMemoryUsage() const
{
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);

    if (!GlobalMemoryStatusEx(&memStatus))
    {
        std::cerr << "GlobalMemoryStatusEx failed." << std::endl;

        return -1.0;
    }

    uint64_t totalPhys = memStatus.ullTotalPhys;
    uint64_t usedPhys  = memStatus.ullTotalPhys - memStatus.ullAvailPhys;
    double percentUsage = (static_cast<double>(usedPhys) / totalPhys) * 100.0;

    return percentUsage;
}


std::map<std::string, double> PerformanceMonitor::getDiskUsage() const
{
    DWORD driveMask = GetLogicalDrives();

    if (driveMask == 0)
    {
        std::cerr << "GetLogicalDrives failed." << std::endl;

        return std::map<std::string, double>();
    }

    std::map<std::string, double> disksUsage;

    for (int i = 0; i < 26; i++) 
    {
        if (driveMask & (1 << i)) 
        {
            char driveLetter = 'A' + i;
            std::string drivePath;
            drivePath.push_back(driveLetter);
            drivePath.append(":\\");

            if (GetDriveTypeA(drivePath.c_str()) != DRIVE_FIXED) 
            {
                continue;
            }

            ULARGE_INTEGER freeBytesAvailable;
            ULARGE_INTEGER totalNumberOfBytes;
            ULARGE_INTEGER totalNumberOfFreeBytes;

            if (!GetDiskFreeSpaceExA(drivePath.c_str(),
                &freeBytesAvailable,
                &totalNumberOfBytes,
                &totalNumberOfFreeBytes))
            {
                std::cerr << "GetDiskFreeSpaceExA failed for " << drivePath << std::endl;

                continue;
            }

            uint64_t usedBytes = totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart;
            double usagePercent = (totalNumberOfBytes.QuadPart > 0) ?
                (static_cast<double>(usedBytes) / totalNumberOfBytes.QuadPart) * 100.0 :
                0.0;

            disksUsage.emplace(drivePath, usagePercent);
        }
    }

    return disksUsage;
}


double PerformanceMonitor::getGPUUsage() const {
    const char* command =
        "nvidia-smi --query-gpu=utilization.gpu,utilization.encoder,utilization.decoder "
        "--format=csv,noheader,nounits";

    FILE* pipe = _popen(command, "r");

    if (!pipe)
    {
        throw std::runtime_error("Failed to open pipe using nvidia-smi");
    }

    char buffer[256];
    std::string result;

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) 
    {
        result += buffer;
    }

    _pclose(pipe);

    std::istringstream iss(result);
    std::string token;
    double values[3] = { 0.0, 0.0, 0.0 };
    int index = 0;

    while (std::getline(iss, token, ',') && index < 3) 
    {
        token.erase(std::remove(token.begin(), token.end(), ' '), token.end());

        try {
            values[index] = std::stod(token);
        }
        catch (...) {
            values[index] = 0.0;
        }

        ++index;
    }

    double aggregated = max(max(values[0], values[1]), values[2] );

    if (aggregated > 100.0)
    {
        aggregated = 100.0;
    }

    return aggregated;
}


double PerformanceMonitor::getNetworkUsage() const 
{
    PDH_HQUERY query;
    PDH_HCOUNTER counter;
    PDH_STATUS status;

    status = PdhOpenQueryA(NULL, 0, &query);

    if (status != ERROR_SUCCESS)
    {
        std::cerr << "PdhOpenQueryA failed with error code: " << status << std::endl;

        return -1.0;
    }

    status = PdhAddCounterA(query, "\\������� �������(*)\\����� ����/�", 0, &counter);

    if (status != ERROR_SUCCESS) 
    {
        std::cerr << "PdhAddCounterA failed for counter \\������� �������(*)\\����� ����/�"
            << " with error code: " << status << std::endl;
        PdhCloseQuery(query);

        return -1.0;
    }

    status = PdhCollectQueryData(query);

    if (status != ERROR_SUCCESS) 
    {
        std::cerr << "First PdhCollectQueryData failed with error code: " << status << std::endl;
        PdhCloseQuery(query);

        return -1.0;
    }

    Sleep(1000);

    status = PdhCollectQueryData(query);

    if (status != ERROR_SUCCESS)
    {
        std::cerr << "Second PdhCollectQueryData failed with error code: " << status << std::endl;
        PdhCloseQuery(query);

        return -1.0;
    }

    DWORD bufferSize = 0;
    DWORD itemCount = 0;
    status = PdhGetFormattedCounterArrayA(counter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, NULL);

    if (status != PDH_MORE_DATA && status != ERROR_SUCCESS) 
    {
        std::cerr << "PdhGetFormattedCounterArrayA failed to get buffer size, error code: " << status << std::endl;
        PdhCloseQuery(query);

        return -1.0;
    }

    std::vector<BYTE> buffer(bufferSize);

    status = PdhGetFormattedCounterArrayA(counter, PDH_FMT_DOUBLE, &bufferSize, &itemCount,
        reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_A*>(buffer.data()));

    if (status != ERROR_SUCCESS) 
    {
        std::cerr << "PdhGetFormattedCounterArrayA failed to retrieve data, error code: " << status << std::endl;
        PdhCloseQuery(query);

        return -1.0;
    }

    double totalNetworkUsage = 0.0;
    PDH_FMT_COUNTERVALUE_ITEM_A* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_A*>(buffer.data());

    for (DWORD i = 0; i < itemCount; i++) 
    {
        totalNetworkUsage += items[i].FmtValue.doubleValue;
    }

    PdhCloseQuery(query);

    return totalNetworkUsage;
}


} // namespace dm
