#pragma once

#include <string>
#include <fstream>
#include <mutex>

namespace Core
{
    enum class LogLevel
    {
        Trace,
        Debug,
        Info,
        Warn,
        Error
    };

    enum class LogCategory {
        Engine,
        Renderer,
        Window,
        Chunk,
        World,
        Noise,
        Mesh,
        Thread
    };

    class Logger
    {
        public:
            static void init(const std::string& logFilePath);
            static void shutdown();

            static void log(LogLevel level,
                                    LogCategory category,
                                    const std::string& message,
                                    const char* file,
                                    int line);
            
            static void setRuntimeLevel(LogLevel level);
            static LogLevel getRuntimeLevel();
        private:
            static std::ofstream s_logFile;
            static std::mutex s_mutex;
            static bool s_initialized;

            static const char* levelToString(LogLevel level);
            static const char* categoryToString(LogCategory category);
    };
}

// ------------------------------------------------------------
// Compile-time filtering (TRACE + DEBUG disabled in Release)
// ------------------------------------------------------------
#ifdef NDEBUG
    #define LOG_TRACE_CAT(cat, msg)
    #define LOG_DEBUG_CAT(cat, msg)
#else
    #define LOG_TRACE_CAT(cat, msg) ::Core::Logger::log(::Core::LogLevel::Trace, cat, msg, __FILE__, __LINE__)
    #define LOG_DEBUG_CAT(cat, msg) ::Core::Logger::log(::Core::LogLevel::Debug, cat, msg, __FILE__, __LINE__)
#endif

#define LOG_INFO_CAT(cat, msg)  ::Core::Logger::log(::Core::LogLevel::Info,  cat, msg, __FILE__, __LINE__)
#define LOG_WARN_CAT(cat, msg)  ::Core::Logger::log(::Core::LogLevel::Warn,  cat, msg, __FILE__, __LINE__)
#define LOG_ERROR_CAT(cat, msg) ::Core::Logger::log(::Core::LogLevel::Error, cat, msg, __FILE__, __LINE__)

// ------------------------------------------------------------
// Category-specific macros
// ------------------------------------------------------------
#define LOG_ENGINE_TRACE(msg)   LOG_TRACE_CAT(::Core::LogCategory::Engine,   msg)
#define LOG_ENGINE_DEBUG(msg)   LOG_DEBUG_CAT(::Core::LogCategory::Engine,   msg)
#define LOG_ENGINE_INFO(msg)    LOG_INFO_CAT (::Core::LogCategory::Engine,   msg)
#define LOG_ENGINE_WARN(msg)    LOG_WARN_CAT (::Core::LogCategory::Engine,   msg)
#define LOG_ENGINE_ERROR(msg)   LOG_ERROR_CAT(::Core::LogCategory::Engine,   msg)

#define LOG_RENDERER_TRACE(msg) LOG_TRACE_CAT(::Core::LogCategory::Renderer, msg)
#define LOG_RENDERER_DEBUG(msg) LOG_DEBUG_CAT(::Core::LogCategory::Renderer, msg)
#define LOG_RENDERER_INFO(msg)  LOG_INFO_CAT (::Core::LogCategory::Renderer, msg)
#define LOG_RENDERER_WARN(msg)  LOG_WARN_CAT (::Core::LogCategory::Renderer, msg)
#define LOG_RENDERER_ERROR(msg) LOG_ERROR_CAT(::Core::LogCategory::Renderer, msg)

#define LOG_CHUNK_TRACE(msg)    LOG_TRACE_CAT(::Core::LogCategory::Chunk,    msg)
#define LOG_CHUNK_DEBUG(msg)    LOG_DEBUG_CAT(::Core::LogCategory::Chunk,    msg)
#define LOG_CHUNK_INFO(msg)     LOG_INFO_CAT (::Core::LogCategory::Chunk,    msg)
#define LOG_CHUNK_WARN(msg)     LOG_WARN_CAT (::Core::LogCategory::Chunk,    msg)
#define LOG_CHUNK_ERROR(msg)    LOG_ERROR_CAT(::Core::LogCategory::Chunk,    msg)

#define LOG_WINDOW_TRACE(msg)   LOG_TRACE_CAT(::Core::LogCategory::Window,   msg)
#define LOG_WINDOW_DEBUG(msg)   LOG_DEBUG_CAT(::Core::LogCategory::Window,   msg)
#define LOG_WINDOW_INFO(msg)    LOG_INFO_CAT (::Core::LogCategory::Window,   msg)
#define LOG_WINDOW_WARN(msg)    LOG_WARN_CAT (::Core::LogCategory::Window,   msg)
#define LOG_WINDOW_ERROR(msg)   LOG_ERROR_CAT(::Core::LogCategory::Window,   msg)
// Add more categories as needed...

// ------------------------------------------------------------
// Assertions
// ------------------------------------------------------------
#if defined(_WIN32)
    #define DEBUG_BREAK() __debugbreak()
#else
    #include <signal.h>
    #define DEBUG_BREAK() raise(SIGTRAP)
#endif

#define ASSERT(expr, msg) \
    do { \
        if (!(expr)) { \
            LOG_ENGINE_ERROR(std::string("Assertion failed: ") + (msg)); \
            DEBUG_BREAK(); \
        } \
    } while (0)