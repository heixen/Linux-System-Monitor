
#include "Core/CpuManager.h"

#include <unistd.h>

#include <cstdint>
#include <mutex>
#include <vector>

#include "Cpu.h"
#include "CpuParser.h"

CPU::CPU() {
    start();
}

void CPU::updateData() {
    std::vector<Core> cpu = GetCpu();
    std::vector<Core> core;

    if (!cpu.empty())
        core.assign(cpu.begin() + 1, cpu.end());

    SetData(m_cpu, cpu);
    SetData(m_core, core);
}

std::vector<Core> CPU::GetCpuUsage() {
    return GetData(m_cpu);
}
std::vector<Core> CPU::GetCoreUsage() {
    return GetData(m_core);
}

float CPU::GetCpuPercentage() {
    std::vector<Core> c = GetData(m_cpu);

    if (c.empty())
        return 0.0f;

    Core cpu = c[0];

    float cpuTotal = 0.0f, idle = 0;

    idle = cpu.idle + cpu.iowait;

    cpuTotal = cpu.user + cpu.nice + cpu.system + cpu.idle + cpu.iowait +
               cpu.irq + cpu.softirq + cpu.steal + cpu.guest + cpu.guest_nice;

    std::lock_guard<std::mutex> lock(m_dataMutex);
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
