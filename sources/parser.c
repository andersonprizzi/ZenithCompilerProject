/////////////////////////////////////////////////////////////////////////////
// LIBRARY INCLUDES                                                        //
/////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "logger.h"
#include "error.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"


#define BACKTRACK_START() marcaPosToken()
#define BACKTRACK_FAIL() restauraPosToken(); return 0
#define BACKTRACK_END() /* Success, no action */
