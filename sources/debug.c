#include "debug.h"
#include "global.h"

void AC71_ReportOperation(const char* msg) {
    if (AC71_DebugMode) printf("%s\n", msg);
}
