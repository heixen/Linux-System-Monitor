#include <fstream>
#include <sstream>
#include <unordered_map>

std::unordered_map<std::string, long> GetMemoryInfo() {
    std::unordered_map<std::string, long> memStat;
    memStat.reserve(57);

    std::ifstream memInfo("/proc/meminfo");
    if (!memInfo.is_open())
        return memStat;

    std::string line;

    while (std::getline(memInfo, line)) {
        std::istringstream iss(line);
        std::string key;
        long value;

        iss >> key >> value;

        if (!key.empty() && key.back() == ':') {
            key.pop_back();
        }
        memStat[key] = value;
    }

    memInfo.close();

    return memStat;
}
