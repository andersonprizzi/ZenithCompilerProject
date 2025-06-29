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



/////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES                                                     //
/////////////////////////////////////////////////////////////////////////////

void CC71_LogMessage(CC71_LogLevel logLevel, const char* msg);

#endif // LOGGER_H_INCLUDED
