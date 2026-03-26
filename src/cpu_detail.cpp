#include "cpu_detail.h"

#include<fstream>
#include<set>
#include<utility>
#include<sstream>

std::string trim(const std::string &s){
    size_t start = s.find_first_not_of(" \t");
    size_t end   = s.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

bool toBool(const std::string& s) {
    return (s == "true" || s == "1");
}

std::string CPUDetail::fetchArchtecture(){
    struct utsname info;
    if(uname(&info) != 0){
        return "unknown";
    }

    return std::string(info.machine);
}

CPUInfo CPUDetail::fetchCPUInfo() {
    CPUInfo data {};
    
    std::ifstream file("/proc/cpuinfo");

    if(!file.is_open()) return data;

    std::set<std::pair<int,int>> cores;
    int physical_id = -1;
    int core_id = -1;
    int logical_threads = 0;

    std::string line;
    bool basic_info_set = false;

    while (std::getline(file, line)) {

        data.has_vmx  = line.find("vmx")  != std::string::npos;
        data.has_svm  = line.find("svm")  != std::string::npos;
        data.has_aes  = line.find("aes")  != std::string::npos;
        data.has_avx2  = line.find("avx2")  != std::string::npos;

        
        if (line.empty()) {
            if (physical_id != -1 && core_id != -1) {
                cores.insert({physical_id, core_id});
            }
            physical_id = core_id = -1;
            continue;
        }

        std::stringstream ss(line);
        std::string key, value;

        if (std::getline(ss, key, ':') && std::getline(ss, value)) {
            key = trim(key);
            value = trim(value);

            // Count logical CPUs
            if (key == "processor") {
                logical_threads++;
                continue;
            }

            // Set common info only once
            if (!basic_info_set) {

                if (key == "vendor_id") {
                    data.vendor_name = value;
                }

                else if (key == "cpu family") {
                    try{
                        data.cpu_family = std::stoi(value);
                    }catch(...){
                        data.cpu_family = 0;
                    }
                }

                else if (key == "model") {
                    try{
                        data.model = std::stoi(value);
                    }catch(...){
                        data.model = 0;
                    }
                }

                else if (key == "model name") {
                    data.model_name = value;
                }

                else if (key == "stepping") {
                    try{

                    data.stepping = std::stoi(value);
                        
                    }catch(...){
                        data.stepping = 0;
                    }
                }

                else if (key == "cache size") {
                    size_t pos = value.find_first_not_of("0123456789");
                    value = value.substr(0, pos);
                    data.cache_kb = std::stol(value);
                }
            }

            // Core mapping
            if (key == "physical id") {
                try{
                    physical_id = std::stoi(value);
                }catch(...){
                    physical_id = 0;
                }
            }

            else if (key == "core id") {
                try{
                    core_id = std::stoi(value);
                }catch(...){
                    core_id = 0;
                }
            }
        }

        // Once we have basic info, stop reassigning
        if (!basic_info_set &&
            !data.model_name.empty() &&
            data.cache_kb != 0) {
            basic_info_set = true;
        }
    }

    if (physical_id != -1 && core_id != -1) {
        cores.insert({physical_id, core_id});
    }

    data.physical_cores = cores.size();
    data.logical_threads = logical_threads;
    data.architecture = fetchArchtecture();
    data.has_smt = logical_threads > data.physical_cores;

    return data;
}

std::string CPUDetail::fetchGovernor(){
    std::ifstream file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
    if(!file.is_open()) return "unknown";
    std::string governor;
    std::getline(file,governor);
    return trim(governor);
}

std::string CPUDetail::fetchBoost(){
    auto config = kBoostPaths.at(vendor_);
    std::ifstream file(config.path);
    if(!file.is_open()) return "N/A";
    std::string boost;
    std::getline(file,boost);
    if(config.inverted == toBool(boost)){
        return "OFF";
    }

    return "ON";
}

LoadAvg CPUDetail::fetchLoad(){
    LoadAvg data {};
    std::ifstream file("/proc/loadavg");
    if(!file.is_open()) return data;

    std::string line;
    std::getline(file,line);
    std::stringstream ss(line);

    ss >> data.load_1min >> data.load_5min >> data.load_15min;

    return data;
}

double CPUDetail::readFreqGHz(std::string core) {
    std::string path = "/sys/devices/system/cpu/"
                     + core
                     + "/cpufreq/scaling_cur_freq";
    std::ifstream f(path);
    long khz = 0;
    f >> khz;
    return khz / 1000000.0;  // KHz → GHz
}

std::string CPUDetail::getDriverName() {
    static const std::unordered_map<std::string, std::string> vendorDriverMap = {
        { "GenuineIntel", "coretemp" },
        { "AuthenticAMD", "k10temp"  },
    };

    auto it = vendorDriverMap.find(vendor_);
    if (it != vendorDriverMap.end())
        return it->second;

    return "";  // unknown vendor
}

std::string CPUDetail::findHwmonPath(const std::string &deriverName){
    for(int i = 0 ;i<=20;i++){
        std::string path = "/sys/class/hwmon/hwmon"+std::to_string(i)+"/name";
        std::ifstream file(path);
        std::string name;
        file >> name;
        if(name == deriverName){
            return "/sys/class/hwmon/hwmon"+std::to_string(i);
        }
    }
    return "";
}


TempSource CPUDetail::resolveTempSource(const std::string& hwmonPath) {

    // looking for "Tdie" first — it's the real die temp with no offset
    for (int i = 1; i <= 10; i++) {
        std::string labelPath = hwmonPath + "/temp" + std::to_string(i) + "_label";
        std::ifstream lf(labelPath);
        if (!lf.is_open()) continue;

        std::string label;
        lf >> label;

        // Tdie — AMD real die temp, no offset needed
        if (label == "Tdie")
            return { hwmonPath + "/temp" + std::to_string(i) + "_input", 0.0 };

        // Package — Intel package temp, no offset needed
        if (label == "Package id 0")
            return { hwmonPath + "/temp" + std::to_string(i) + "_input", 0.0 };
    }

    // Step 2 — Tdie not found, fall back to Tctl (AMD only)
    // Tctl is a virtual control temp — some Ryzen chips add +27°C offset
    // Heuristic: read current value, if >60°C at idle → offset present
    std::string tctlPath = hwmonPath + "/temp1_input";
    std::ifstream tf(tctlPath);
    if (!tf.is_open()) return { "", 0.0 };  // nothing found

    long millideg = 0;
    tf >> millideg;
    double tctl = millideg / 1000.0;

    // if Tctl reads suspiciously high at idle → subtract 27°C offset
    double offset = (tctl > 60.0) ? -27.0 : 0.0;
    return { tctlPath, offset };
}

void CPUDetail::initTempSource(){
    std::string hwmon_path_ = findHwmonPath(getDriverName());

    if(hwmon_path_.empty()) {
        tempsource_ = {"",0.0};
        return;
    }

    tempsource_ = resolveTempSource(hwmon_path_);
}

double CPUDetail::readPackageTemp(){
    if(tempsource_.path.empty()) return -1.0;

    std::ifstream file(tempsource_.path);
    if(!file.is_open()) return -1.0;

    double millideg = 0;
    file >> millideg;

    return (millideg / 1000.0) + tempsource_.offset;

}

std::vector<CacheLevel> CPUDetail::readCacheInfo(){
    std::vector<CacheLevel> caches;
    std::string base = "/sys/devices/system/cpu/cpu0/cache";
    for(int i = 0; ;i++){
        std::string path = base + "/index" + std::to_string(i) + "/";
        
        std::ifstream lf(path+"level");
        if(!lf.is_open()) break;

        CacheLevel c;
        lf >> c.level;

        std::ifstream tf(path+"type");
        tf >> c.type;

        std::ifstream sf(path + "size");
        std::string sizeKb;
        sf >> sizeKb;
        c.sizeKB = std::stoi(sizeKb);
        caches.push_back(c);
    }
    return caches;
}

std::string CPUDetail::formatCacheInfo(const std::vector<CacheLevel>& caches) {
    std::string result;
    for (const auto& c : caches) {
        std::string label = "L" + std::to_string(c.level);

        // differentiate L1d and L1i
        if (c.type == "Data")        label += "d";
        else if (c.type == "Instruction") label += "i";

        // format size — show MB if >= 1024K
        std::string size;
        if (c.sizeKB >= 1024)
            size = std::to_string(c.sizeKB / 1024) + "MB";
        else
            size = std::to_string(c.sizeKB) + "K";

        result += label + ": " + size + "  ";
    }
    return result;
}

Scheduler CPUDetail::fetchScheduler(){
    std::ifstream file("/proc/stat");
    if(!file.is_open()) return {};

    Scheduler schedule {};
    std::string line;
    while(std::getline(file,line)){
        std::string key = "";
        std::stringstream ss(line);
        ss >> key;
        if(key == "intr"){
            ss >> schedule.irqs_per_sec;
            continue;
        }
        if(key == "ctxt"){
            ss >> schedule.ctxt_per_sec;
            continue;
        }
        if(key == "procs_running"){
            ss >> schedule.procs_running;
            continue;
        }
        if(key == "procs_blocked"){
            ss >> schedule.procs_blocked;
            continue;
        }
    }
    return schedule;
}

void CPUDetail::update() {
    if (first_run_) {
        curr_       = fetchCPUInfo();
        vendor_     = curr_.vendor_name;

        initTempSource();
        prev_cpu_map_ = readCPUStats();

        caches = readCacheInfo();

        curr_.caches_info = formatCacheInfo(caches);  

        prev_scheduler_ = fetchScheduler();
        first_run_  = false;
        return;  
    }

   
    LoadAvg load          = fetchLoad();
    curr_policy_.governor = fetchGovernor();
    curr_policy_.boost    = fetchBoost();
    curr_policy_.load_1min   = load.load_1min;
    curr_policy_.load_5min   = load.load_5min;
    curr_policy_.load_15min  = load.load_15min;

   
    auto curr_cpu_map = readCPUStats();

    double temp = readPackageTemp();

    auto _scheduler = fetchScheduler();

    curr_scheduler_.ctxt_per_sec = (_scheduler.ctxt_per_sec - prev_scheduler_.ctxt_per_sec)/1;
    curr_scheduler_.irqs_per_sec = (_scheduler.irqs_per_sec - prev_scheduler_.irqs_per_sec)/1;
    curr_scheduler_.procs_running = _scheduler.procs_running;
    curr_scheduler_.procs_blocked = _scheduler.procs_blocked;

    std::unordered_map<std::string, CpuMetrics> curr_usage_map;

    for (const auto& [label, curr_stats] : curr_cpu_map) {
        if(label =="cpu") continue;
        auto it = prev_cpu_map_.find(label);
        if (it == prev_cpu_map_.end()) continue;

        auto delta = cpu->calculateDelta(it->second, curr_stats);
        curr_usage_map[label].usage_percent = CalculateUsage(delta);
        curr_usage_map[label].frequency_ghz = readFreqGHz(label);
    }
    curr_usage_map["cpu"].temperature_c = temp;
    prev_cpu_map_ = curr_cpu_map; 
    prev_scheduler_ = _scheduler; 
    usage_map     = curr_usage_map;
}

double  CPUDetail::CalculateUsage(const CPUStats& delta) {

    long long totalIdle = delta.idle + delta.iowait;
        long long totalNonIdle = delta.user + delta.nice + delta.system + delta.irq + delta.softirq + delta.steal;
        long long total = totalIdle + totalNonIdle;

        if (total == 0.0) return 0.0; // avoid division by zero

       
     double utilization = 100.0 * (1.0 - static_cast<double>(totalIdle) / total);
     return utilization;
 }



const CPUInfo CPUDetail::getCPUInfo()const{
    return curr_;
}

const SystemPolicy CPUDetail::getSystemPolicy() const{
    return curr_policy_;
}

const std::unordered_map<std::string,CpuMetrics> CPUDetail::getUsagePerCore() const{
    return usage_map;
}

const Scheduler CPUDetail::getScheduler() const{
    return curr_scheduler_;
}


