
#pragma once

#include "cpu.h"

#include<string>
#include<unordered_map>
#include<vector>

#include<sys/utsname.h>

struct CacheLevel {
    int         level;  // 1, 2, 3
    std::string type;   // Data, Instruction, Unified
    int         sizeKB; // in KB
};

struct Scheduler{
    long ctxt_per_sec;    // context switches per second
    long irqs_per_sec;    // hardware interrupts per second
    int  procs_running;   // processes actively on CPU
    int  procs_blocked;   // processes waiting for I/O
};


struct TempSource {
    std::string path;
    double offset;
};

struct CpuMetrics {
    double usage_percent;
    double frequency_ghz;
    double temperature_c;
};

struct SystemPolicy{
    std::string governor;
    std::string boost;
    float load_1min;
    float load_5min;
    float load_15min;
};

struct BoostConfig {
    std::string path;
    bool inverted;   // true = Intel (0 means ON), false = AMD (1 means ON)
};

struct LoadAvg{
     float load_1min;
    float load_5min;
    float load_15min;
};


struct CPUInfo {
    int         model;
    std::string vendor_name;
    std::string model_name;
    std::string architecture;
    int         cpu_family;
    int         stepping;
    int         physical_cores;   // cpu cores (physical)
    int         logical_threads;  // siblings (with SMT/HT)
    long        cache_kb;
    std::string caches_info;      // formatted: "L1d: 32K  L1i: 32K  L2: 512K  L3: 8MB"

    // CPU feature flags (read from /proc/cpuinfo flags line)
    bool has_vmx;    // Intel VT-x hardware virtualization
    bool has_svm;    // AMD-V hardware virtualization (your chip)
    bool has_aes;    // AES-NI hardware accelerated encryption
    bool has_avx2;   // AVX2 SIMD instructions (ML/video)
    bool has_smt;    // SMT/Hyperthreading (logical_threads > physical_cores)

};

class CPUDetail{
    private:

    std::string vendor_;
    bool first_run_ = true;

    CPUInfo curr_;
    SystemPolicy curr_policy_;
    CPUUsage total_cpu_usage;

    const std::unordered_map<std::string, BoostConfig> kBoostPaths = {
    { "AuthenticAMD", { "/sys/devices/system/cpu/cpufreq/boost",              false } },
    { "GenuineIntel", { "/sys/devices/system/cpu/intel_pstate/no_turbo",      true  } },
};

    CPUInfo fetchCPUInfo();
    std::string fetchArchtecture();
    std::string fetchGovernor();
    std::string fetchBoost();
    LoadAvg     fetchLoad();

    double CalculateUsage(const CPUStats& delta);
    std::unordered_map<std::string, CPUStats> read_cpu_stats();
    std::unordered_map<std::string , CpuMetrics > usage_map;
    std::unordered_map<std::string, CPUStats> prev_cpu_map_;

    double readFreqGHz(std::string core);

    std::string getDriverName();
    std::string findHwmonPath(const std::string& driverName);
    TempSource resolveTempSource(const std::string& hwmonPath);
    TempSource tempsource_;
    void initTempSource();
    double readPackageTemp();

    std::vector<CacheLevel> caches;
    std::vector<CacheLevel> readCacheInfo();
    std::string formatCacheInfo(const std::vector<CacheLevel>& caches);

    Scheduler prev_scheduler_;
    Scheduler curr_scheduler_;
    Scheduler fetchScheduler();

    CPUMonitor* cpu;

    public:
    CPUDetail(CPUMonitor* c) : cpu(c) {};

    const CPUInfo getCPUInfo() const;
    const SystemPolicy getSystemPolicy() const;
    const std::unordered_map<std::string, CpuMetrics> getUsagePerCore()const ;
    const Scheduler getScheduler() const;
    void update();

    
};