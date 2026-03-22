#pragma once
#include<chrono>
#include<fstream>
#include<string>
#include<iomanip>
#include<sstream>
class sysInfo{
    public:
    std::string currentTime();
    std::string getupTime();
    std::string format_ps_time(double runtime_sec);
};
