#pragma once
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
   
    bool firstRun = true;  
    CPUStats prevStats;

    
    CPUUsage currentUsage;

   friend class CPUDetail;
    
    CPUStats calculateDelta(const CPUStats& prev,
                            const CPUStats& curr);

    unsigned long long getTotalDelta(const CPUStats& delta);

    CPUUsage calculateUsage(const CPUStats& delta);

public:
    void update();
    
    const CPUUsage& getUsage() const;
};

/* 1. Delta = Change between two measurements. */