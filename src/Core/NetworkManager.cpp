#include "NetworkManager.h"

#include <array>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "NetParser.h"
#include "NetworkManager.h"

NetworkManager::NetworkManager() {}

NetworkManager::~NetworkManager() {
    m_running = false;
    if (m_NetworkManagerThreads.joinable())
        m_NetworkManagerThreads.join();
}

void NetworkManager::start_() {
    if (m_running)
        return;
    m_running = true;

    m_NetworkManagerThreads = std::thread(&NetworkManager::update_, this);
    std::cout << "rah\n";
}

void NetworkManager::update_() {
    using namespace std::literals::chrono_literals;
    while (m_running) {
        std::vector<NetworkInterface> interface = GetNetworkInfo();
        {
            std::lock_guard<std::mutex> lock(m_NetworkManagerMutex);
            m_interface = interface;
        }
        std::this_thread::sleep_for(1s);
    }
};

std::vector<NetworkInterface> NetworkManager::GetNetwork() {
    std::lock_guard<std::mutex> lock(m_NetworkManagerMutex);
    return m_interface;
}
