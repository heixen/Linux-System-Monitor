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
        int i = 0;
        std::istringstream iss(line);
        iss >> _;

        if (_ == "intr") {
            break;
        }
        // std::cout << line << std::endl;
        Core core;
        for (int i = 0; i < 10; i++) {
            iss >> core.stat[i];
        }
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
