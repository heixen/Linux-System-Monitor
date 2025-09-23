#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "Cpu.h"

std::vector<Core> GetCpu() {
    std::ifstream stream("/proc/stat");
    std::string line;

    std::string _;

    std::vector<Core> cpu;
    cpu.reserve(4);

    if (!stream) {
        std::cout << "faild to open /proc/stat\n";
        return cpu;
    }

    while (getline(stream, line)) {
        std::istringstream iss(line);
        iss >> _;

        if (_ == "intr") {
            break;
        }
        Core core;
        iss >> core.user >> core.nice >> core.system >> core.idle >>
            core.iowait >> core.irq >> core.softirq >> core.steal >>
            core.guest >> core.guest_nice;

        cpu.emplace_back(core);
    }
    stream.close();

    return cpu;
}

struct CPU {
    char *Arch;
    char *Vendor;
    char *Model;
    unsigned int ThreadsPerCore;
    int Cores;
};

void GetCpuInfo() {}
