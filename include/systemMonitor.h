#pragma once

#include "cpu.h"
#include "ResourceMonitor.h"
#include "process.h"
#include "network_monitor.h"
#include "cpu_detail.h"

class SystemMonitor {
private:
    CPUMonitor cpu;
    ResourceMonitor resource;
    ProcessMonitor processMonitor;
    NetworkMonitor networkMonitor;
    CPUDetail cpuDetail;
    

public:
    SystemMonitor() : cpuDetail(&cpu) {}
    void update();

    const CPUUsage& getCPUUsage() const;

    const MemoryUsage& getMemoryInfo() const;

    const DiskUsage& getDiskInfo() const;

    const std::vector<Process>& getProcesses() const;

    const NetworkInfo& getNetStat() const;

     int getTotalProcess()const ;

     int getTotalZombieProcess() const ;

    void sortProcessBy(SortBy criterion);

    //--flags
    const CPUInfo  getCPUInfo() const;

    const SystemPolicy getSystemPolicy() const;

    const Scheduler getScheduler() const;

    const std::unordered_map<std::string, CpuMetrics> getUsagePerCore()const ;
};