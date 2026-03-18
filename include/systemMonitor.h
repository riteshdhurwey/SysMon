#pragma once

#include "cpu.h"
#include "ResourceMonitor.h"
#include "process.h"
#include "network_monitor.h"

class SystemMonitor {
private:
    CPUMonitor cpu;
    ResourceMonitor resource;
    ProcessMonitor processMonitor;
    NetworkMonitor networkMonitor;
    

public:
    void update();

    const CPUUsage& getCPUUsage() const;

    const MemoryUsage& getMemoryInfo() const;

    const DiskUsage& getDiskInfo() const;

    const std::vector<Process>& getProcesses() const;

    const NetworkInfo& getNetStat() const;

    void sortProcessBy(SortBy criterion);
};