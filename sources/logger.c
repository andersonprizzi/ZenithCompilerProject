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
    return;
    /*const char *levelStr;
    const char *color;

    switch (logLevel) {
        case CC71_LOG_DEBUG:   levelStr = "DEBUG";   color = ANSI_COLOR_CYAN; break;
        case CC71_LOG_INFO:    levelStr = "INFO";    color = ANSI_COLOR_GREEN; break;
        case CC71_LOG_WARNING: levelStr = "WARNING"; color = ANSI_COLOR_YELLOW; break;
        case CC71_LOG_ERROR:   levelStr = "ERROR";   color = ANSI_COLOR_RED; break;
        default:               levelStr = "LOG";     color = ANSI_COLOR_RESET; break;
    }

    FILE *out;
    int is_file = 0;

    if (CC71_GlobalOutputErrorFile) {
        out = CC71_GlobalOutputErrorFile;
        color = "";
        is_file = 1;
    } else {
        out = stderr;
        is_file = 0;
    }

    const char *color_magenta = is_file ? "" : ANSI_COLOR_MAGENTA;
    const char *color_yellow = is_file ? "" : ANSI_COLOR_YELLOW;
    const char *color_green = is_file ? "" : ANSI_COLOR_GREEN;
    const char *color_red = is_file ? "" : ANSI_COLOR_RED;
    const char *color_reset = is_file ? "" : ANSI_COLOR_RESET;

    va_list args;

    if (logEvent != CC71_LOG_EVENT_GENERIC) {
        va_start(args, format);

        switch (logEvent) {
            case CC71_LOG_EVENT_ACCEPTED_TOKEN: {
                int token = va_arg(args, int);
                const char* lexeme = va_arg(args, const char*);
                fprintf(out, "%s[%s]%s [ACCEPTED TOKEN................]%s %d ('%s')\n",
                        color, levelStr, color_magenta, color_reset, token, lexeme);
                break;
            }
            case CC71_LOG_EVENT_ENTER_FUNCTION: {
                const char* funcName = va_arg(args, const char*);
                fprintf(out, "%s[%s]%s [ENTERING FUNCTION.............]%s %s\n", color, levelStr, color_yellow, color_reset, funcName);
                break;
            }
            case CC71_LOG_EVENT_EXIT_SUCCESS: {
                const char* funcName = va_arg(args, const char*);
                fprintf(out, "%s[%s]%s [EXITING FUNCTION (SUCCESS)....]%s %s\n", color, levelStr, color_green, color_reset, funcName);
                break;
            }
            case CC71_LOG_EVENT_EXIT_FAILURE: {
                const char* funcName = va_arg(args, const char*);
                fprintf(out, "%s[%s]%s [EXITING FUNCTION (FAILURE)....]%s %s\n", color, levelStr, color_red, color_reset, funcName);
                break;
            }
            default:
                break;
        }

        va_end(args);

    } else {
        va_start(args, format);
        fprintf(out, "%s[%s]%s ", color, levelStr, color_reset);
        vfprintf(out, format, args);
        fprintf(out, "\n");
        va_end(args);
    }

    fflush(out);
    */
}
