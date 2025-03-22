#include "Logger.h"

#include <cstdarg>
#include <iostream>
#include <string>

static std::string logLevelStrings[] = { "ERROR: ", "WARNING: ", "INFO: ", "DEBUG: " };

void Log(LogLevel logLevel, char const* format, ...)
{
    char buffer[4096];
    strcpy(buffer, logLevelStrings[logLevel].c_str());

    va_list args;
    va_start(args, format);
    vsprintf(buffer + logLevelStrings[logLevel].length(), format, args);
    va_end(args);

    std::cout << buffer << std::endl;
}
