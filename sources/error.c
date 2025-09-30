/////////////////////////////////////////////////////////////////////////////
// LIBRARY INCLUDES                                                        //
/////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "logger.h"
#include "error.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"

extern const char* tokens[];



/////////////////////////////////////////////////////////////////////////////
// ERROR TABLE                                                             //
/////////////////////////////////////////////////////////////////////////////

static const CC71_ErrorEntry CC71_ErrorTable[CC71_ERR_COUNT] = {
    { CC71_ERR_LEX_INVALID_CHAR,        CC71_ERROR_LEXICAL, "CC71_ERR_LEX_INVALID_CHAR",        "Invalid character encountered." },
    { CC71_ERR_LEX_UNCLOSED_STRING,     CC71_ERROR_LEXICAL, "CC71_ERR_LEX_UNCLOSED_STRING",     "Unclosed string literal." },
    { CC71_ERR_LEX_INVALID_NUMBER,      CC71_ERROR_LEXICAL, "CC71_ERR_LEX_INVALID_NUMBER",      "Invalid numeric literal." },

    { CC71_ERR_SYN_UNEXPECTED_TOKEN,    CC71_ERROR_SYNTAX,  "CC71_ERR_SYN_UNEXPECTED_TOKEN",    "Unexpected token found." },
    { CC71_ERR_SYN_MISSING_SEMICOLON,   CC71_ERROR_SYNTAX,  "CC71_ERR_SYN_MISSING_SEMICOLON",   "Missing semicolon." },
    { CC71_ERR_SYN_UNMATCHED_BRACKET,   CC71_ERROR_SYNTAX,  "CC71_ERR_SYN_UNMATCHED_BRACKET",   "Unmatched bracket." },
    { CC71_ERR_SYN_EXPECTED_OPEN_PAREN, CC71_ERROR_SYNTAX,  "CC71_ERR_SYN_EXPECTED_OPEN_PAREN", "Expected '('." },
    { CC71_ERR_SYN_EXPECTED_CLOSE_PAREN,CC71_ERROR_SYNTAX,  "CC71_ERR_SYN_EXPECTED_CLOSE_PAREN","Expected ')'." },
    { CC71_ERR_SYN_EXPECTED_EOF,        CC71_ERROR_SYNTAX,  "CC71_ERR_SYN_EXPECTED_EOF",        "Expected end of file." },

    { CC71_ERR_SEM_UNDECLARED_IDENTIFIER, CC71_ERROR_SEMANTIC, "CC71_ERR_SEM_UNDECLARED_IDENTIFIER", "Undeclared identifier." },
    { CC71_ERR_SEM_REDECLARATION,         CC71_ERROR_SEMANTIC, "CC71_ERR_SEM_REDECLARATION",         "Redeclaration of identifier." },
    { CC71_ERR_SEM_TYPE_MISMATCH,         CC71_ERROR_SEMANTIC, "CC71_ERR_SEM_TYPE_MISMATCH",         "Type mismatch." },
    { CC71_ERR_SEM_INVALID_RETURN,        CC71_ERROR_SEMANTIC, "CC71_ERR_SEM_INVALID_RETURN",        "Invalid return statement." },

    { CC71_ERR_FATAL_OUT_OF_MEMORY, CC71_ERROR_FATAL, "CC71_ERR_FATAL_OUT_OF_MEMORY", "Out of memory." },
    { CC71_ERR_FATAL_INTERNAL,      CC71_ERROR_FATAL, "CC71_ERR_FATAL_INTERNAL",      "Internal compiler error." }
};



/////////////////////////////////////////////////////////////////////////////
// FUNCTION IMPLEMENTATIONS                                                //
/////////////////////////////////////////////////////////////////////////////

const CC71_ErrorEntry* CC71_GetErrorEntry(CC71_ErrorCode code) {
    if (code >= 0 && code < CC71_ERR_COUNT) {
        return &CC71_ErrorTable[code];
    }
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////

const char* CC71_GetErrorType(CC71_ErrorType type) {
    switch (type) {
        case CC71_ERROR_LEXICAL: return "Lexical Error";
        case CC71_ERROR_SYNTAX: return "Syntax Error";
        case CC71_ERROR_SEMANTIC: return "Semantic Error";
        case CC71_ERROR_FATAL: return "Fatal Error";
        default: return "Unknown Error Type";
    }
}

/////////////////////////////////////////////////////////////////////////////

/*int CC71_ReportError(CC71_ErrorCode code, int line, int column, const char* fmt, ...) {
    const CC71_ErrorEntry* entry = CC71_GetErrorEntry(code);
    const char* errorType = entry ? CC71_GetErrorType(entry->type) : "Unknown Error";

    if (!entry) {
        fprintf(CC71_GlobalOutputErrorFile, "[%d:%d] Unknown error code %d.\n", line, column, code);
        fclose(CC71_GlobalOutputErrorFile);
        return 1;
    }

    if (fmt) {
        va_list args;
        va_start(args, fmt);

        fprintf(CC71_GlobalOutputErrorFile, "[%s][%d:%d] ", errorType, line, column);
        vfprintf(CC71_GlobalOutputErrorFile, fmt, args);
        fprintf(CC71_GlobalOutputErrorFile, "\n");
        fflush(CC71_GlobalOutputErrorFile);

        va_end(args);
    } else {
        fprintf(CC71_GlobalOutputErrorFile, "[%s][%d:%d] %s\n", errorType, line, column, entry->message);
        fflush(CC71_GlobalOutputErrorFile);
    }

    fclose(CC71_GlobalOutputErrorFile);

    if (entry->type == CC71_ERROR_FATAL) {
        exit(EXIT_FAILURE);
    }

    return 0;
}*/

int CC71_FLAG_ERROR = 0;
int CC71_FLAG_FORCE = 0;

int CC71_ReportError(CC71_ErrorCode code, int line, int column, const char* fmt, ...) {
    if (CC71_SilentMode > 0 && !CC71_FLAG_FORCE) return; // FIX ME

    const CC71_ErrorEntry* entry = CC71_GetErrorEntry(code);
    const char* errorType = entry ? CC71_GetErrorType(entry->type) : "Unknown Error";

    if (!CC71_FLAG_ERROR) {
        if (!entry) {
            fprintf(stderr, "[%d:%d] Unknown error code %d.\n", line, column, code);
            return 1;
        }

        if (fmt) {
            va_list args;
            va_start(args, fmt);

            fprintf(stderr, "[%s][%d:%d] ", errorType, line, column);
            vfprintf(stderr, fmt, args);
            fprintf(stderr, "\n");
            fflush(stderr);

            va_end(args);
        } else {
            fprintf(stderr, "[%s][%d:%d] %s\n", errorType, line, column, entry->message);
            fflush(stderr);
        }

        if (entry->type == CC71_ERROR_FATAL) {
            exit(EXIT_FAILURE);
        }

        CC71_FLAG_ERROR = 1;
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * @brief Reports a syntax error when an unexpected token is encountered.
 *
 * This function compares the expected token with the current token and logs an error message
 * if they do not match. It respects the global silent mode used during backtracking to avoid
 * logging errors that are part of exploratory parsing attempts, unless forced.
 *
 * @param expectedToken The token that was expected.
 * @param ... (Optional) One integer argument (force): if non-zero, forces error reporting even in silent mode.
 */
void CC71_ReportExpectedTokenError(int expectedToken, ...) {
    int force = 0;

    va_list args;
    va_start(args, expectedToken);

    // Se a chamada tiver um segundo argumento (int), será extraído aqui.
    force = va_arg(args, int);

    va_end(args);

    CC71_FLAG_FORCE = force;

    // Respeita o modo silencioso, exceto se `force` for verdadeiro.
    if (CC71_SilentMode > 0 && !CC71_FLAG_FORCE) return;

    
    CC71_ReportError(
        CC71_ERR_SYN_UNEXPECTED_TOKEN,
        CC71_GlobalCurrentLine,
        columnAux,
        "Expected '%s' but found '%s'.",
        CC71_TokenToString(expectedToken),
        CC71_TokenToString(CC71_GlobalTokenNumber)
    );
}


/////////////////////////////////////////////////////////////////////////////

const char* CC71_TokenToString(int token) {
    if (token >= 0 && token < TokenCount && tokens[token]) {
        return tokens[token];
    }
    return "Invalid token";
}
