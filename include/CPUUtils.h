#pragma once
#include <string>
#include <fstream>
#include <sstream>

struct CPUStats {
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
};

// Reads /proc/stat and returns a CPUStats snapshot
inline CPUStats readCPUStats() {
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line); // first line is "cpu ..."

    CPUStats stats{};
    std::istringstream ss(line);

    std::string cpuLabel;
    ss >> cpuLabel
       >> stats.user
       >> stats.nice
       >> stats.system
       >> stats.idle
       >> stats.iowait
       >> stats.irq
       >> stats.softirq
       >> stats.steal;

    return stats;
}