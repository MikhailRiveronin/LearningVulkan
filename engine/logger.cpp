#include "logger.h"

void log(Log_Level level, char const* format, ...)
{
    std::stringstream ss;
    switch (level)
    {
        case Log_Level::ERROR:
            ss << "Error: ";
            break;

        case Log_Level::WARNING:
            ss << "Warning: ";
            break;

        case Log_Level::INFO:
            ss << "Info: ";
            break;

        default:
            break;
    }

    char buffer[256];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    std::string output = ss.str() + buffer;
    std::cout << output;
}
