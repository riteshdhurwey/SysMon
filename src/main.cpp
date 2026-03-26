#include "ui.h"
#include <chrono>

int main(int argc , char* argv[]) {
    SystemMonitor monitor;
    UI ui;
    ui.init();

    monitor.update();
    for(int i = 0;i<argc;i++){
        std::string arg = argv[i];
        if(arg == "--cpu") ui.setdetailed_cpu_enabled();
    }
    auto lastUpdate = std::chrono::steady_clock::now();

    while (true) {
    if (!ui.handleInput(monitor)) break;

    auto now = std::chrono::steady_clock::now();
    auto idleTime = std::chrono::duration_cast<std::chrono::milliseconds>
                    (now - ui.getLastKeyTime()).count();

    bool shouldUpdate = !ui.isPaused() || idleTime >= 1000;

    if (shouldUpdate && now - lastUpdate >= std::chrono::seconds(1)) {
        if (idleTime >= 1000) ui.setPaused(false);
        monitor.update();
        lastUpdate = now;
    }

    ui.draw(monitor);  
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
}

    ui.destroy();
    return 0;
}