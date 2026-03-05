#include "display.h"
#include <thread>
#include <chrono>

int main()
{
    SystemMonitor monitor;
    Display display;

    while(true)
    {
        std::cout << "\033[2J\033[H";  // Clear screen & move cursor to above
        monitor.update();

        display.show(monitor);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}