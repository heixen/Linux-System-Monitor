#include "MemoryManager.h"

#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "MemoryParser.h"

Memory::Memory() : m_running(false) {}

Memory::~Memory() {
    if (m_MemoryThread.joinable())
        m_MemoryThread.join();

    m_running = false;
}

void Memory::start_() {
    if (m_running)
        return;

    m_running = true;

    m_MemoryThread = std::thread(&Memory::update_, this);
    std::cout << "mem iu\n";
}

void Memory::update_() {
    using namespace std::literals::chrono_literals;
    while (m_running) {
        std::unordered_map<std::string, long> mem = GetMemoryInfo();
        {
            std::lock_guard<std::mutex> lock(m_MemoryMutex);
            m_Memory.swap(mem);
        }
        std::this_thread::sleep_for(1s);
    }
}

const std::unordered_map<std::string, long> Memory::GetMemoryUsage() const {
    std::lock_guard<std::mutex> lock(m_MemoryMutex);
    return m_Memory;
}
