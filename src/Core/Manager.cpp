#include "Manager.h"

#include <thread>

Manager::Manager() = default;

Manager::~Manager() {
    stop();
}

void Manager::start() {
    if (m_running.load())
        return;

    m_running = true;
    m_thread = std::thread(&Manager::update, this);
}

void Manager::stop() {
    m_running = false;
    if (m_thread.joinable())
        m_thread.join();
}

void Manager::update() {
    using namespace std::literals::chrono_literals;
    while (m_running) {
        updateData();
        std::this_thread::sleep_for(1s);
    }
}
