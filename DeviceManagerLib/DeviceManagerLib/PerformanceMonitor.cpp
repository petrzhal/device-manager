#include "pch.h"
#include "PerformanceMonitor.h"


namespace dm
{

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

        status = PdhAddEnglishCounterA(cpuQuery, "\\Processor(_Total)\\% Processor Time", 0, &cpuTotal);

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

        std::this_thread::sleep_for(std::chrono::seconds(2));

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
        uint64_t usedPhys = memStatus.ullTotalPhys - memStatus.ullAvailPhys;
        double percentUsage = (static_cast<double>(usedPhys) / totalPhys) * 100.0;

        return percentUsage;
    }

    PerformanceMonitor::DisksStorage PerformanceMonitor::getDiskUsage() const
    {
        const auto updateDiskInfo = [](const std::string &disk) -> DisksType
        {
            ULARGE_INTEGER freeBytesAvailable;
            ULARGE_INTEGER totalNumberOfBytes;
            ULARGE_INTEGER totalNumberOfFreeBytes;

            if (!GetDiskFreeSpaceExA(disk.c_str(), &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes))
            {
                std::println(std::cerr, "GetDiskFreeSpaceEx failed for {}", disk);

                return {};
            }

            uint64_t usedBytes = totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart;
            float usagePercent = (totalNumberOfBytes.QuadPart > 0)
                                     ? (static_cast<float>(usedBytes) / totalNumberOfBytes.QuadPart) * 100.0f
                                     : 0.0f;

            return std::make_pair(disk, usagePercent);
        };

        static const auto driveMask = GetLogicalDrives();

        if (driveMask == 0)
        {
            std::println(std::cerr, "GetLogicalDrives failed");

            return DisksStorage{};
        }

        static const auto existedDisks = std::views::iota(0, ('Z' - 'A') + 1) | std::views::filter([&](int i) -> bool
                                                                                                   { return driveMask & (1 << i); }) |
                                         std::views::transform([](int diskN) -> std::string
                                                               { return std::format("{}:\\", static_cast<char>('A' + diskN)); }) |
                                         std::views::filter([](std::string disk) -> bool
                                                            { return GetDriveTypeA(disk.data()) == DRIVE_FIXED; }) |
                                         std::ranges::to<std::vector>();

        return existedDisks | std::views::transform(updateDiskInfo) | std::ranges::to<DisksStorage>();
    }

    double PerformanceMonitor::getGPUUsage() const
    {
        const char *command =
            "nvidia-smi --query-gpu=utilization.gpu,utilization.encoder,utilization.decoder "
            "--format=csv,noheader,nounits";

        FILE *pipe = _popen(command, "r");

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
        double values[3] = {0.0, 0.0, 0.0};
        int index = 0;

        while (std::getline(iss, token, ',') && index < 3)
        {
            token.erase(std::remove(token.begin(), token.end(), ' '), token.end());

            try
            {
                values[index] = std::stod(token);
            }
            catch (...)
            {
                values[index] = 0.0;
            }

            ++index;
        }

        double aggregated = max(max(values[0], values[1]), values[2]);

        if (aggregated > 100.0)
        {
            aggregated = 100.0;
        }

        return aggregated;
    }

    int PerformanceMonitor::getNetworkUsage() const
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

        status = PdhAddEnglishCounterA(query, "\\Network Interface(*)\\Bytes Total/sec", 0, &counter);

        if (status != ERROR_SUCCESS)
        {
            std::cerr << "PdhAddCounterA failed for counter \\Network Interface(*)\\Bytes Total/sec"
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
                                              reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_A *>(buffer.data()));

        if (status != ERROR_SUCCESS)
        {
            std::cerr << "PdhGetFormattedCounterArrayA failed to retrieve data, error code: " << status << std::endl;
            PdhCloseQuery(query);

            return -1.0;
        }

        double totalNetworkUsage = 0.0;
        PDH_FMT_COUNTERVALUE_ITEM_A *items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_A *>(buffer.data());

        for (DWORD i = 0; i < itemCount; i++)
        {
            totalNetworkUsage += items[i].FmtValue.doubleValue;
        }

        PdhCloseQuery(query);

        return static_cast<int>(totalNetworkUsage);
    }

} // namespace dm
