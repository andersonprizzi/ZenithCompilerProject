#include "logger.h"
#include "global.h"

void CC71_LogMessage(CC71_LogLevel logLevel, const char* msg) {
    if (AC71_DebugMode) {
        if (logLevel == LOG_INFO) printf("[INFO] %s\n", msg);
    }
}
