#pragma once

#include "defines.h"

enum class Log_Level
{
    ERROR,
    WARNING,
    INFO
};

void log(Log_Level level, char const* format, ...);

#define LOG_ERROR(format, ...) log(Log_Level::ERROR, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) log(Log_Level::WARNING, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) log(Log_Level::INFO, format, ##__VA_ARGS__)
