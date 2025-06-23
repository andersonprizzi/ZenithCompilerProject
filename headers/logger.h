#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_NONE
} CC71_LogLevel;

void CC71_LogMessage(CC71_LogLevel logLevel, const char* msg);

#endif // LOGGER_H_INCLUDED
