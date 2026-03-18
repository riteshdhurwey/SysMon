#include"network_monitor.h"

std::vector<RawNetData> NetworkMonitor::readProcNetDev(){
    std::vector<RawNetData> result;
    std::ifstream file("/proc/net/dev");
    std::string line;
    int line_no = 0;
    while(std::getline(file,line)){
        RawNetData data {};
        line_no++;
        if(line_no<=2) continue;

        std::stringstream ss(line);


        ss >> data.iface;
        data.iface.pop_back();

        ss >> data.rx_bytes;
        for(int i=0;i<7;i++) ss >> line;

        ss >> data.tx_bytes;

        result.push_back(data);
    }

    return result;
}

NetworkStat NetworkMonitor::computeSpeed(RawNetData prev,RawNetData curr){
    NetworkStat stat;
    stat.interface = curr.iface;
    stat.rx_bytes = curr.rx_bytes;
    stat.tx_bytes = curr.tx_bytes;

    stat.rxKBps = (curr.rx_bytes - prev.rx_bytes) / 1024.0;
    stat.txKBps = (curr.tx_bytes - prev.tx_bytes) / 1024.0;
    
    return stat;
}

int NetworkMonitor::countTCPConnections() {
    int count = 0;

    for (const auto& path : {tcpPath, tcpPath+'6'}) {
        std::ifstream file(path);
        if (!file) continue;
        std::string line;
        std::getline(file, line);  // skip header

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string sl, local, remote, state;
            ss >> sl >> local >> remote >> state;
            if (state == "01") count++;  // ESTABLISHED
        }
    }
    return count;
}

int NetworkMonitor::countUDPConnections(){
    std::ifstream file(udpPath);
    std::string line;

    int count = -1; // skip header

    while(std::getline(file, line)) {
        count++;
    }

    return count;
}


void NetworkMonitor::update(){
    std::vector<RawNetData> currSnap = readProcNetDev();

    if(firstRun){
        prevSnap = currSnap;
        firstRun = false;
        current.tcpConnections = countTCPConnections();
        current.udpConnections = countUDPConnections();
        return;
    }

    current.stat.clear();

    for(int i = 0; i<(int)currSnap.size();i++){
        NetworkStat stat = computeSpeed(prevSnap[i],currSnap[i]);
        current.stat.push_back(stat);
    }

    current.tcpConnections = countTCPConnections();
    current.udpConnections = countUDPConnections();
    prevSnap = currSnap;
}

const NetworkInfo& NetworkMonitor::getNetInfo() const{
    return current;
}