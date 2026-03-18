#include <map>
#include <string>
#include <vector>
#include<sstream>
#include<fstream>

static const std::string udpPath = "/proc/net/udp";
 const std::string tcpPath = "/proc/net/tcp";

struct NetworkStat {
    std::string interface;
    unsigned long long rx_bytes;
    unsigned  long long tx_bytes;
    double rxKBps;  // download
    double txKBps;  // upload
};

struct RawNetData {
    std::string iface;
    unsigned long long rx_bytes;
    unsigned long long tx_bytes;
};

struct NetworkInfo {
    std::vector<NetworkStat> stat;
    int tcpConnections = 0; //ESTABLISHED
    int udpConnections = 0;
};

class NetworkMonitor {
private:
    NetworkInfo current;

    bool firstRun = true;

    std::vector<RawNetData> prevSnap;

    std::vector<RawNetData> readProcNetDev();
    NetworkStat computeSpeed(RawNetData prev,RawNetData curr);

    int countTCPConnections();
    int countUDPConnections();


public:
    void update();
    const NetworkInfo& getNetInfo() const;
};