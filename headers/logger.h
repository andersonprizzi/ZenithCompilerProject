#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

/////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS                                                        //
/////////////////////////////////////////////////////////////////////////////

typedef enum {
    CC71_LOG_DEBUG,   // Detailed internal information for debugging.
    CC71_LOG_INFO,    // General information about compiler progress.
    CC71_LOG_WARNING, // Non-critical issues that may indicate problems.
    CC71_LOG_ERROR,   // Critical issues that prevent continued execution.
} CC71_LogLevel;

typedef enum {
    CC71_LOG_EVENT_GENERIC,
    CC71_LOG_EVENT_ENTER_FUNCTION,
    CC71_LOG_EVENT_ACCEPTED_TOKEN,
    CC71_LOG_EVENT_BACKTRACK_START,
    CC71_LOG_EVENT_EXIT_SUCCESS,
    CC71_LOG_EVENT_EXIT_FAILURE
} CC71_LogEvent;



/////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES                                                     //
/////////////////////////////////////////////////////////////////////////////

void CC71_LogMessage(CC71_LogLevel logLevel, CC71_LogEvent logEvent, const char* format, ...);

#endif // LOGGER_H_INCLUDED
