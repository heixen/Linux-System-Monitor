
#include "Core/CpuManager.h"

#include <unistd.h>

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
            m_cpu.swap(cpu);
            // std::vector<Core> core(cpu.begin() + 1, cpu.end());
            // m_core.swap(core);
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

    float cpuTotal = 0, idle = 0;

    idle = c[0].stat[3] + c[0].stat[4];

    for (int &i : c[0].stat) {
        cpuTotal += i;
    }
    // initial r
    if (this->m_total == 0) {
        this->m_total = cpuTotal;
        this->m_idle = idle;

        return 0.0f;
    }
    // calculate
    float totalDelta = cpuTotal - this->m_total;
    float idleDelta = idle - this->m_idle;

    this->m_total = cpuTotal;
    this->m_idle = idle;

    if (totalDelta <= 0.0f)
        return 0.0f;

    float usage = (totalDelta - idleDelta) / totalDelta * 100;

    return usage;
}

std::vector<Core> CPU::GetCores() {
    std::vector<Core> c = CPU::GetCpuUsage();

    if (!c.empty())
        c.erase(c.begin());

    return c;
}
