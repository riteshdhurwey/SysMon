#include <iostream>   // std::cout
#include <fstream>    // std::ifstream
#include <string>     // std::string
#include <thread>     // std::this_thread::sleep_for
#include <chrono>     // std::chrono::seconds
#include <cstdio>     // sscanf
#include <unistd.h>
#include"CPUUtils.h"

//Compute data

struct CPUUsage {
    double user;
    double nice;
    double system;
    double idle;
    double iowait;
    double irq;
    double softirq;
    double steal;
    double total;
};

class CPUMonitor {
private:
   
    bool firstRun = true;  // Helpful while computing usage
    CPUStats prevStats;

    // Stores usage in percentage
    CPUUsage currentUsage;

   // CPUStats readStat();
    // Compute usage between two small time frame
    CPUStats calculateDelta(const CPUStats& prev,
                            const CPUStats& curr);
        // raw data of totoal usage of CPU
    unsigned long long getTotalDelta(const CPUStats& delta);
        // convert raw data into percentage 
    CPUUsage calculateUsage(const CPUStats& delta);

public:
    void update();
    
    const CPUUsage& getUsage() const;
};

/* 1. Delta = Change between two measurements. */