
#include "Core/CpuManager.h"

#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

#include "Cpu.h"
#include "CpuParser.h"

CPU::CPU() : m_running(false), m_total(0), m_idle(0) {}

void CPU::start_() {
    if (m_running)
        return;

    m_CpuThread = std::thread(&CPU::update_, this);
    std::cout << "anh iu\n";
}

CPU::~CPU() {
    m_running = false;
    if (m_CpuThread.joinable())
        m_CpuThread.join();
}

void CPU::update_() {
    using namespace std::literals::chrono_literals;

    m_running = true;

    while (m_running) {
        std::vector<Core> cpu = GetCpu();
        {
            std::lock_guard<std::mutex> lock(m_CpuMutex);
            std::vector<Core> core;

            if (!cpu.empty()) {
                core.assign(cpu.begin() + 1, cpu.end());
            }

            m_cpu.swap(cpu);
            m_core.swap(core);
        }
        std::this_thread::sleep_for(1s);
    }
}

std::vector<Core> CPU::GetCpuUsage() {
    std::lock_guard<std::mutex> lock(m_CpuMutex);
    return m_cpu;
}
std::vector<Core> CPU::GetCoreUsage() {
    std::lock_guard<std::mutex> lock(m_CpuMutex);
    return m_core;
}

float CPU::GetCpuPercentage() {
    std::vector<Core> c = CPU::GetCpuUsage();

    if (c.empty())
        return 0.0f;

    Core cpu = c[0];

    float cpuTotal = 0.0f, idle = 0;

    idle = cpu.idle + cpu.iowait;

    cpuTotal = cpu.user + cpu.nice + cpu.system + cpu.idle + cpu.iowait +
               cpu.irq + cpu.softirq + cpu.steal + cpu.guest + cpu.guest_nice;
    // initial r
    if (m_total == 0) {
        m_total = cpuTotal;
        m_idle = idle;

        return 0.0f;
    }
    // calculate
    uint64_t totalDelta = cpuTotal - m_total;
    uint64_t idleDelta = idle - m_idle;

    m_total = cpuTotal;
    m_idle = idle;

    if (totalDelta <= 0.0f)
        return 0.0f;

    float usage =
        static_cast<float>(totalDelta - idleDelta) / totalDelta * 100.0f;

    return usage;
}

std::vector<Core> CPU::GetCores() {
    std::vector<Core> c = CPU::GetCpuUsage();

    if (!c.empty())
        c.erase(c.begin());

    return c;
}
