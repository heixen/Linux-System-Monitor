#pragma once
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#include "Network.h"

class NetworkManager {
   public:
    NetworkManager();
    ~NetworkManager();

    void start_();
    void update_();

    std::vector<NetworkInterface> GetNetwork();

   private:
    std::vector<NetworkInterface> m_interface;
    std::atomic<bool> m_running = false;
    std::mutex m_NetworkManagerMutex;
    std::thread m_NetworkManagerThreads;
};
