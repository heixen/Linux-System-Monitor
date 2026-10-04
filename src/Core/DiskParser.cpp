
#include <fstream>
#include <iostream>

void GetDiskStat() {
    std::ifstream stream("/proc/diskstats");
    std::string _line;

    std::cout << _line << std::endl;
}
