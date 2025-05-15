#ifndef LOG_H
#define LOG_H

#include <cstdio>
#include <string>

enum LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

#ifdef ENABLE_DEBUG
#define LOG(level, msg, ...) \
    printf("[%s][%s:%s():%d] " msg "\n", logLevelToString(level), getFileName(__FILE__).c_str(), __func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG(level, msg, ...)                                                                                                             \
    if (level != DEBUG)                                                                                                                  \
    {                                                                                                                                    \
        printf("[%s][%s:%s():%d] " msg "\n", logLevelToString(level), getFileName(__FILE__).c_str(), __func__, __LINE__, ##__VA_ARGS__); \
    }
#endif

inline const char *logLevelToString(LogLevel level)
{
    switch (level)
    {
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "INFO";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

inline const std::string getFileName(const std::string &filePath)
{
    size_t pos = filePath.find_last_of("/\\");
    if (pos != std::string::npos)
        return filePath.substr(pos + 1);
    return filePath;
}

const std::string getLogFileName();

#endif // LOG_H
