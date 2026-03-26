#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include<unordered_map>

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
inline std::unordered_map<std::string,CPUStats> readCPUStats() {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open /proc/stat");
    }

    std::unordered_map<std::string, CPUStats> cpu_map;
    cpu_map.reserve(16); // small optimization

    std::string line;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string label;
        ss >> label;

        // stop when CPU section ends
        if (label.rfind("cpu", 0) != 0) break;

        CPUStats stats{};
        ss >> stats.user
           >> stats.nice
           >> stats.system
           >> stats.idle
           >> stats.iowait
           >> stats.irq
           >> stats.softirq
           >> stats.steal;

        cpu_map[label] = stats;
    }

    return cpu_map;
}