#pragma once

#include <atomic>
#include <mutex>
#include <thread>

class Manager {
   public:
    Manager();
    virtual ~Manager();

    void start();
    void stop();

   protected:
    virtual void updateData() = 0;

    template <typename T>
    void SetData(T& newDataMember, const T& dataMember) {
        std::lock_guard<std::mutex> lock(m_dataMutex);
        newDataMember.swap(const_cast<T&>(dataMember));
    }

    template <typename T>
    T GetData(const T& dataMember) {
        std::lock_guard<std::mutex> lock(m_dataMutex);
        return dataMember;
    }

   private:
    void update();
    mutable std::mutex m_dataMutex;
    std::thread m_thread;
    std::atomic<bool> m_running{false};
};
