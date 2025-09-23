#include <dirent.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Process.h"

std::vector<Process> GetProcess() {
    std::vector<Process> processes;
    processes.reserve(512);
    DIR* dir = opendir("/proc");

    if (dir == nullptr) {
        std::cout << "failed to open /proc\n";
        return processes;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            std::string _program;
            std::string _cmd;
            std::string _statm;
            unsigned int resident_mem;
            unsigned int virtual_mem;
            unsigned int shared_mem;
            int thread_count;

            int pid = atoi(entry->d_name);

            std::ifstream comm("/proc/" + std::string(entry->d_name) + "/comm");
            if (comm.is_open()) {
                std::getline(comm, _program);
            }
            std::ifstream cmd("/proc/" + std::string(entry->d_name) +
                              "/cmdline");
            if (cmd.is_open()) {
                std::getline(cmd, _cmd);
            }

            std::ifstream stat("/proc/" + std::string(entry->d_name) + "/stat");
            if (stat.is_open()) {
            }

            // size, resident, shared, text, lib, data, dt
            std::ifstream statm("/proc/" + std::string(entry->d_name) +
                                "/statm");
            if (statm.is_open()) {
                std::getline(statm, _statm);

                std::istringstream iss(_statm);

                unsigned int size, res, shared;

                iss >> size >> res >> shared;

                // mem_kb = (size * 4096) + (res * 4096) + (shared * 4096);
                virtual_mem = size;
                resident_mem = res;
                shared_mem = shared;
            }
            std::ifstream status("/proc/" + std::string(entry->d_name) +
                                 "/status");
            if (status.is_open()) {
                std::string line;
                while (std::getline(status, line)) {
                    if (line.find("Threads:") == 0) {
                        thread_count =
                            std::stoi(line.substr(line.find(':') + 1));
                    }
                }
            }

            processes.emplace_back(pid, std::move(_program), std::move(_cmd),
                                   resident_mem, virtual_mem, shared_mem,
                                   thread_count);
        }
    }
    closedir(dir);
    return processes;
}
