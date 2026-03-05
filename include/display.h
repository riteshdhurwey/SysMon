#pragma once
#include"systemMonitor.h"
class Display {
public:
    void show(const SystemMonitor& monitor);

private:
    void showCPU(const CPUUsage& cpu);
    void showMemory(const MemoryUsage& mem);
    void showDisk(const DiskUsage& disk);
    void showProcesses(const std::vector<Process>& processes);
};