#pragma once
#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

class Memory {
   public:
    Memory();
    ~Memory();

    void update_();
    void start_();
    const std::unordered_map<std::string, long> GetMemoryUsage() const;

   private:
    std::unordered_map<std::string, long> m_Memory;
    std::atomic<bool> m_running;
    mutable std::mutex m_MemoryMutex;
    std::thread m_MemoryThread;
};
