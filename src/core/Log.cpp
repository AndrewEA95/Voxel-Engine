#include "Log.h"

#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <sstream>

#if defined (_WIN32)
    #include <windows.h>
#endif

namespace  
{
    void setColor(Core::LogLevel level) 
    {
        #if defined(_WIN32)
            HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
            WORD color = 7;

            switch (level) 
            {
                case Core::LogLevel::Trace: color = 11; break; // bright cyan
                case Core::LogLevel::Debug: color = 9;  break; // bright blue
                case Core::LogLevel::Info:  color = 15; break; // bright white
                case Core::LogLevel::Warn:  color = 14; break; // yellow
                case Core::LogLevel::Error: color = 12; break; // red
            }

            SetConsoleTextAttribute(h, color);
        #else
            switch (level) 
            {
                case Core::LogLevel::Trace: std::cout << "\033[96m"; break;
                case Core::LogLevel::Debug: std::cout << "\033[94m"; break;
                case Core::LogLevel::Info:  std::cout << "\033[97m"; break;
                case Core::LogLevel::Warn:  std::cout << "\033[93m"; break;
                case Core::LogLevel::Error: std::cout << "\033[91m"; break;
            }
        #endif
    }

    void resetColor() 
    {
        #if defined(_WIN32)
        {
            HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(h, 7);
        }
        #else
        {
            std::cout << "\033[0m";
        }
        #endif
    }
}

namespace Core 
{
    std::ofstream Logger::s_logFile;
    std::mutex Logger::s_mutex;
    bool Logger::s_initialized = false;
    static LogLevel s_runtimeLevel = LogLevel::Trace; // defalut

    void Logger::init(const std::string &logFilePath)
    {
        std::lock_guard<std::mutex> lock(s_mutex);

        if(s_initialized)
        {
            return;
        }

        s_logFile.open(logFilePath, std::ios::out | std::ios::trunc);
        if(!s_logFile.is_open())
        {
            std::cerr << "[Logger] Failed to open log file: " << logFilePath << "\n";
        }
        else 
        {
            s_initialized = true;
            // Check VOXEL_LOG_LEVEL
            if (const char* env = std::getenv("VOXEL_LOG_LEVEL"))
            {
                std::string level = env;

                if (level == "TRACE") s_runtimeLevel = LogLevel::Trace;
                else if (level == "DEBUG") s_runtimeLevel = LogLevel::Debug;
                else if (level == "INFO")  s_runtimeLevel = LogLevel::Info;
                else if (level == "WARN")  s_runtimeLevel = LogLevel::Warn;
                else if (level == "ERROR") s_runtimeLevel = LogLevel::Error;
            }

            // Individual toggles override VOXEL_LOG_LEVEL
            if (std::getenv("VOXEL_LOG_TRACE") == nullptr)
            {
                if (s_runtimeLevel == LogLevel::Trace)
                    s_runtimeLevel = LogLevel::Debug;
            }

            if (std::getenv("VOXEL_LOG_DEBUG") == nullptr)
            {
                if (s_runtimeLevel == LogLevel::Debug)
                    s_runtimeLevel = LogLevel::Info;
            }
        }
    }

    void Logger::shutdown()
    {
        std::lock_guard<std::mutex> lock(s_mutex);

        if(s_logFile.is_open())
        {
            s_logFile.flush();
            s_logFile.close();
        }

        s_initialized = false;
    }

    const char* Logger::levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case Core::LogLevel::Debug: return "DEBUG";
            case LogLevel::Info:  return "INFO";
            case LogLevel::Warn:  return "WARN";
            case LogLevel::Error: return "ERROR";
            default: return "UNKNOWN";
        }
    }

    const char* Logger::categoryToString(LogCategory cat) 
    {
        switch (cat) 
        {
            case LogCategory::Engine:   return "Engine";
            case LogCategory::Renderer: return "Renderer";
            case LogCategory::Window:   return "Window";
            case LogCategory::Chunk:    return "Chunk";
            case LogCategory::World:    return "World";
            case LogCategory::Noise:    return "Noise";
            case LogCategory::Mesh:     return "Mesh";
            case LogCategory::Thread:   return "Thread";
        }
        return "Unknown";
    }

    void Logger::setRuntimeLevel(LogLevel level) 
    {
        s_runtimeLevel = level;
    }

    LogLevel Logger::getRuntimeLevel() 
    {
        return s_runtimeLevel;
    }

    void Logger::log(LogLevel level,
                 LogCategory category,
                 const std::string& message,
                 const char* file,
                 int line
                )
    {
        std::lock_guard<std::mutex> lock(s_mutex);

        // Runtime filtering
        if (level < s_runtimeLevel)
            return;

        // Timestamp
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
    #if defined(_WIN32)
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif

        std::ostringstream ts;
        ts << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

        // Build prefix
        std::ostringstream prefix;
        prefix << "[" << levelToString(level) << "] "
            << "[" << categoryToString(category) << "] ";

        // Console output
        setColor(level);
        std::cout << prefix.str();
        
        resetColor();
        std::cout << ts.str() << " "
                << file << ":" << line << " - "
                << message << "\n";

        // File output (no color)
        if (s_initialized) {
            s_logFile << prefix.str()
                    << ts.str() << " "
                    << file << ":" << line << " - "
                    << message << "\n";
            s_logFile.flush();
        }
    }
}