#pragma once
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#include "Network.h"

class Network {
   public:
    Network();
    ~Network();

    void start_();
    void update_();

    std::vector<NetworkInterface> GetNetwork();

   private:
    std::vector<NetworkInterface> m_interface;
    std::atomic<bool> m_running = false;
    std::mutex m_NetworkMutex;
    std::thread m_NetworkThreads;
};
