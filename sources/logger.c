/////////////////////////////////////////////////////////////////////////////
// LIBRARY INCLUDES                                                        //
/////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "logger.h"
#include "error.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"

void CC71_LogMessage(CC71_LogLevel logLevel, const char* msg) {
    char str[20];

    if (logLevel == CC71_LOG_DEBUG) strcpy(str, "DEBUG");
    else if (logLevel == CC71_LOG_INFO) strcpy(str, "INFO");
    else if (logLevel == CC71_LOG_WARNING) strcpy(str, "WARNING");
    else if (logLevel == CC71_LOG_ERROR) strcpy(str, "ERROR");

    printf("[%s] %s\n", str, msg);
}
