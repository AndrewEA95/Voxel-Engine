#pragma once

#include "Log.h"
#include <chrono>

#ifndef NDEBUG
    #define PROFILE_SCOPE(name) ::Core::ProfileTimer timer##__LINE__(name)
#else
    #define PROFILE_SCOPE(name)
#endif

namespace Core {

class ProfileTimer {
public:
    ProfileTimer(const char* name)
        : m_name(name),
          m_start(std::chrono::high_resolution_clock::now()) {}

    ~ProfileTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration<double, std::milli>(end - m_start).count();
        LOG_ENGINE_INFO(std::string("[PROFILE] ") + m_name + " took " + std::to_string(ms) + " ms");
    }

private:
    const char* m_name;
    std::chrono::high_resolution_clock::time_point m_start;
};

} // namespace Core