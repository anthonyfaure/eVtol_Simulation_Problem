#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>

inline std::mutex& log_mutex() {
    static std::mutex m;
    return m;
}

template<typename... Args>
void log(Args&&... args) {
    std::lock_guard lock(log_mutex());
    auto now = std::chrono::system_clock::now();
    auto tt  = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif
    std::cout << "[" << std::put_time(&tm, "%H:%M:%S") << "] ";
    (std::cout << ... << std::forward<Args>(args));
    std::cout << '\n';
}
