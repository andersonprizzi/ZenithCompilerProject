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
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"



/////////////////////////////////////////////////////////////////////////////
// FUNCTION IMPLEMENTATIONS                                                //
/////////////////////////////////////////////////////////////////////////////

void CC71_LogMessage(CC71_LogLevel logLevel, CC71_LogEvent logEvent, const char* format, ...) {
    const char *levelStr;
    const char *eventStr; 
    const char *color;

    switch (logLevel) {
        case CC71_LOG_DEBUG:   levelStr = "DEBUG";   color = ANSI_COLOR_CYAN; break;
        case CC71_LOG_INFO:    levelStr = "INFO";    color = ANSI_COLOR_GREEN; break;
        case CC71_LOG_WARNING: levelStr = "WARNING"; color = ANSI_COLOR_YELLOW; break;
        case CC71_LOG_ERROR:   levelStr = "ERROR";   color = ANSI_COLOR_RED; break;
        default:               levelStr = "LOG";     color = ANSI_COLOR_RESET; break;
    }

    va_list args;
    va_start(args, format);

    switch (logEvent) {
        case CC71_LOG_EVENT_ACCEPTED_TOKEN: {
            int token = va_arg(args, int);
            const char* lexeme = va_arg(args, const char*);            
            fprintf(stderr, "%s[%s]%s [ACCEPTED TOKEN................]%s %d ('%s')\n", color, levelStr, ANSI_COLOR_MAGENTA, ANSI_COLOR_RESET, token, lexeme);
            break;
        }
        case CC71_LOG_EVENT_ENTER_FUNCTION: {
            const char* funcName = va_arg(args, const char*);
            fprintf(stderr, "%s[%s]%s [ENTERING FUNCTION.............]%s %s\n", color, levelStr, ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, funcName);
            break;
        }
        case CC71_LOG_EVENT_EXIT_SUCCESS: {
            const char* funcName = va_arg(args, const char*);
            fprintf(stderr, "%s[%s]%s [EXITING FUNCTION (SUCCESS)....]%s %s\n", color, levelStr, ANSI_COLOR_GREEN, ANSI_COLOR_RESET, funcName);
            break;
        }
        case CC71_LOG_EVENT_EXIT_FAILURE: {
            const char* funcName = va_arg(args, const char*);
            fprintf(stderr, "%s[%s]%s [EXITING FUNCTION (FAILURE)....]%s %s\n", color, levelStr, ANSI_COLOR_RED, ANSI_COLOR_RESET, funcName);
            break;
        }
        default: {
            fprintf(stderr, "%s[%s]%s ", color, levelStr, ANSI_COLOR_RESET);
            vfprintf(stderr, format, args);
            fprintf(stderr, "\n");
        }
    }

    fflush(stderr);
    va_end(args);
}
