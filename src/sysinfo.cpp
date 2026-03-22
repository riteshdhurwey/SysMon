#include"sysinfo.h"

std::string sysInfo::currentTime(){
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%a %d %b %Y  %H:%M:%S", localtime(&now));
    return std::string(buf);
}

std::string sysInfo::getupTime(){
    std::ifstream file("/proc/uptime");
    double uptimeSeconds;
    file >> uptimeSeconds;

    int days    = uptimeSeconds / 86400;
    int hours   = ((int)uptimeSeconds % 86400) / 3600;
    int minutes = ((int)uptimeSeconds % 3600) / 60;
    int seconds = (int)uptimeSeconds % 60;

    char buf[64];
    snprintf(buf, sizeof(buf), "%dd %02d:%02d:%02d", days, hours, minutes, seconds);
    return std::string(buf);
}

std::string sysInfo::format_ps_time(double runtime_sec) {
    int total = static_cast<int>(runtime_sec);

    int days = total / 86400;
    int hours = (total % 86400) / 3600;
    int minutes = (total % 3600) / 60;
    int seconds = total % 60;

    std::ostringstream out;


    out << std::setw(2) << std::setfill('0') << days << "-"
        << std::setw(2) << std::setfill('0') << hours << ":"
        << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds;

    return out.str();
}