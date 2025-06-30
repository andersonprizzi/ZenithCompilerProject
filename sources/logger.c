/////////////////////////////////////////////////////////////////////////////
// LIBRARY INCLUDES                                                        //
/////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "logger.h"
#include "error.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"



/////////////////////////////////////////////////////////////////////////////
// PREPROCESSING DEFINITIONS                                               //
/////////////////////////////////////////////////////////////////////////////

#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"



/////////////////////////////////////////////////////////////////////////////
// FUNCTION IMPLEMENTATIONS                                                //
/////////////////////////////////////////////////////////////////////////////

void CC71_LogMessage(CC71_LogLevel logLevel, const char* format, ...) {
    const char* levelStr;
    const char* color;

    switch (logLevel) {
        case CC71_LOG_DEBUG:   levelStr = "DEBUG";   color = ANSI_COLOR_CYAN; break;
        case CC71_LOG_INFO:    levelStr = "INFO";    color = ANSI_COLOR_GREEN; break;
        case CC71_LOG_WARNING: levelStr = "WARNING"; color = ANSI_COLOR_YELLOW; break;
        case CC71_LOG_ERROR:   levelStr = "ERROR";   color = ANSI_COLOR_RED; break;
        default:               levelStr = "LOG";     color = ANSI_COLOR_RESET; break;
    }

    va_list args;
    va_start(args, format);

    fprintf(stderr, "%s[%s]%s ", color, levelStr, ANSI_COLOR_RESET);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);

    va_end(args);
}
