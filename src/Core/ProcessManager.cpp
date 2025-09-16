#include "ProcessManager.h"

#include <iostream>
#include <thread>
#include <vector>

#include "Process.h"
#include "ProcessParser.h"

ProcessManager::ProcessManager() : m_running(false) {}

ProcessManager::~ProcessManager() {
    m_running = false;
    if (m_processThread.joinable()) {
        m_processThread.join();
    }
}

void ProcessManager::start_() {
    if (m_running)
        return;
    m_running = true;

    m_processThread = std::thread(&ProcessManager::update_, this);
    std::cout << "ypa\n";
}

void ProcessManager::update_() {
    using namespace std::literals::chrono_literals;
    std::cout << "..\n";

    while (m_running) {
        std::vector<Process> process = GetProcess();
        {
            std::lock_guard<std::mutex> lock(m_ProcessMutex);
            // m_process = std::move(process);
            m_process.swap(process);
        }
        std::this_thread::sleep_for(1s);
    }
}

std::vector<Process> ProcessManager::get_process() {
    std::lock_guard<std::mutex> lock(m_ProcessMutex);
    return m_process;
}
