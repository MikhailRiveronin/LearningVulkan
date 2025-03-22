#pragma once

typedef enum {
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG
} LogLevel;

constexpr char const* errorIndent = "       ";
constexpr char const* warningIndent = "         ";
constexpr char const* infoIndent = "      ";
constexpr char const* debugIndent = "       ";

void Log(LogLevel logLevel, char const* format, ...);

#define LOG_ERROR(format, ...) Log(LOG_LEVEL_ERROR, format, ## __VA_ARGS__)
#define LOG_WARNING(format, ...) Log(LOG_LEVEL_WARNING, format, ## __VA_ARGS__)
#define LOG_INFO(format, ...) Log(LOG_LEVEL_INFO, format, ## __VA_ARGS__)
#define LOG_DEBUG(format, ...) Log(LOG_LEVEL_DEBUG, format, ## __VA_ARGS__)
