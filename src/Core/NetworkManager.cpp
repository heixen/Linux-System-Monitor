#include "NetworkManager.h"

#include <array>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "NetParser.h"

Network::Network() {}

Network::~Network() {
    m_running = false;
    if (m_NetworkThreads.joinable())
        m_NetworkThreads.join();
}

void Network::start_() {
    if (m_running)
        return;
    m_running = true;

    m_NetworkThreads = std::thread(&Network::update_, this);
    std::cout << "rah\n";
}

void Network::update_() {
    using namespace std::literals::chrono_literals;
    while (m_running) {
        std::vector<NetworkInterface> interface = GetNetworkInfo();
        {
            std::lock_guard<std::mutex> lock(m_NetworkMutex);
            m_interface = interface;
        }
        std::this_thread::sleep_for(1s);
    }
};

std::vector<NetworkInterface> Network::GetNetwork() {
    std::lock_guard<std::mutex> lock(m_NetworkMutex);
    return m_interface;
}
