#include"systemMonitor.h"
const CPUUsage& SystemMonitor::getCPUUsage()const{
        return cpu.getUsage();
}

const MemoryUsage& SystemMonitor::getMemoryInfo() const{
    return resource.getMemory();
}

const DiskUsage& SystemMonitor::getDiskInfo() const{
    return resource.getDisks();
}

const std::vector<Process> & SystemMonitor::getProcesses() const{
    return processMonitor.getProcesses();
}

void SystemMonitor::update(){
    cpu.update();
    resource.update();
    processMonitor.update();
    networkMonitor.update();

    cpuDetail.update();
}

const NetworkInfo& SystemMonitor::getNetStat() const{
    return networkMonitor.getNetInfo();
}

void SystemMonitor::sortProcessBy(SortBy criterion){
    processMonitor.sortBy(criterion);
}

 int SystemMonitor::getTotalProcess() const{
    return processMonitor.getTotoalProcess();
 }

  int SystemMonitor::getTotalZombieProcess() const{
    return processMonitor.getZombieProcess();
 }

 const CPUInfo SystemMonitor::getCPUInfo() const{
    return cpuDetail.getCPUInfo();
 }

 const SystemPolicy SystemMonitor::getSystemPolicy() const{
    return cpuDetail.getSystemPolicy();
 }

 const std::unordered_map<std::string, CpuMetrics> SystemMonitor::getUsagePerCore()const {
    return cpuDetail.getUsagePerCore();
}

const Scheduler SystemMonitor::getScheduler() const{
    return cpuDetail.getScheduler();
}