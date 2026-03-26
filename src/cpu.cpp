#include"cpu.h"

CPUStats CPUMonitor::calculateDelta(const CPUStats &prev,const CPUStats &curr){
    CPUStats diff{};
    diff.user = curr.user - prev.user;
    diff.nice = curr.nice - prev.nice;
    diff.system = curr.system - prev.system;
    diff.idle = curr.idle - prev.idle;
    diff.iowait = curr.iowait - prev.iowait;
    diff.irq = curr.irq - prev.irq;
    diff.softirq = curr.softirq - prev.softirq;
    diff.steal = curr.steal - prev.steal;
    return diff;
}

unsigned long long CPUMonitor::getTotalDelta(const CPUStats &delta){
    return delta.user+delta.nice+delta.system+delta.idle+delta.iowait+delta.irq+delta.softirq+delta.steal;
}


CPUUsage CPUMonitor::calculateUsage(const CPUStats& diff) {

    unsigned long long total = getTotalDelta(diff);

    CPUUsage data{};

    if (total == 0)
        return data;

    data.user    = 100.0 * diff.user    / total;
    data.nice    = 100.0 * diff.nice    / total;
    data.system  = 100.0 * diff.system  / total;
    data.idle    = 100.0 * diff.idle    / total;
    data.iowait  = 100.0 * diff.iowait  / total;
    data.irq     = 100.0 * diff.irq     / total;
    data.softirq = 100.0 * diff.softirq / total;
    data.steal   = 100.0 * diff.steal   / total;

    unsigned long long idle_time = diff.idle + diff.iowait;

    data.total = (1.0 - double(idle_time) / total) * 100.0;

    return data;
}

const CPUUsage &CPUMonitor::getUsage()const{
    return currentUsage;
}

void CPUMonitor::update(){
    auto cpu_map = readCPUStats();
    CPUStats curr = cpu_map["cpu"];
    if(firstRun){
        prevStats = curr;
        firstRun = false;
        return;
    }
    CPUStats delta = calculateDelta(prevStats,curr);

    currentUsage = calculateUsage(delta);

    prevStats = curr;
}