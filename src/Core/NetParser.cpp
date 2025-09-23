
#include <dirent.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Network.h"

std::vector<NetworkInterface> GetNetworkInfo() {
    std::string line;
    std::vector<NetworkInterface> interface;
    interface.reserve(2);

    DIR* dir = opendir("/sys/class/net");

    if (dir == nullptr) {
        std::cerr << "failed to open /sys/class/net";
        return interface;
    }

    struct dirent* entry;

    while ((entry = readdir(dir)) != nullptr) {
        unsigned long rxBytes;
        unsigned long txBytes;

        std::string name = entry->d_name;

        if (name == "." || name == "..")
            continue;

        std::ifstream rx("/sys/class/net/" + std::string(name) +
                         "/statistics/rx_bytes");
        if (rx.is_open()) {
            std::getline(rx, line);
            rxBytes = std::stoul(line);
        }

        std::ifstream tx("/sys/class/net/" + std::string(name) +
                         "/statistics/tx_bytes");
        if (tx.is_open()) {
            std::getline(tx, line);
            txBytes = std::stoul(line);
        }

        interface.emplace_back(name, rxBytes, txBytes);
    }

    closedir(dir);

    return interface;
}
