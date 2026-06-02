#pragma once
#include <vector>

#include "Manager.h"
#include "Models/Cpu.h"

class CPU : public Manager {
   public:
    CPU();
    ~CPU() override = default;

    // const char* GetModel() const {}
    // int GetCoreCount() const;
    // float GetClockSpeedGHz() const {}

    std::vector<Core> GetCpuUsage();
    std::vector<Core> GetCoreUsage();
    float GetCpuPercentage();
    std::vector<Core> GetCores();

   private:
    void updateData() override;

    std::vector<Core> m_cpu;
    std::vector<Core> m_core;
    float m_total;
    float m_idle;
};
