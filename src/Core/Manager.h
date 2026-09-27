#pragma once

#include <atomic>
#include <mutex>
#include <thread>
#include <utility>

class Manager {
   public:
    Manager();
    virtual ~Manager();

    void start();
    void stop();

   protected:
    virtual void updateData() = 0;
    mutable std::mutex m_dataMutex;

    template <typename T>
    void SetData(T& destination, const T&& source) {
        std::lock_guard<std::mutex> lock(m_dataMutex);
        destination = std::move(source);
    }

    template <typename T>
    T GetData(const T& dataMember) {
        std::lock_guard<std::mutex> lock(m_dataMutex);
        return dataMember;
    }

   private:
    void update();
    std::thread m_thread;
    std::atomic<bool> m_running{false};
};
