#include "error.h"
#include "global.h"

/////////////////////////////////////////////////////////////////////////////

static const AC71_ErrorEntry AC71_ErrorTable[] = {
    {   AC71_ERR_INFO_SUCCESS,
        AC71_ERROR_INFO,
        "AC71_ERR_INFO_SUCCESS",
        "Operation completed successfully."
    },
    {
        AC71_ERR_WARNING_UNUSED_VAR,
        AC71_ERROR_WARNING,
        "AC71_ERR_WARNING_UNUSED_VAR",
        "Variable declared but never used."
    },
    { AC71_ERR_LEX_INVALID_CHAR,   AC71_ERROR_LEXICAL, "AC71_ERR_LEX_INVALID_CHAR",    "Invalid character encountered." },
    { AC71_ERR_LEX_UNCLOSED_STRING,AC71_ERROR_LEXICAL, "AC71_ERR_LEX_UNCLOSED_STRING", "Unclosed string literal." },
    { AC71_ERR_LEX_INVALID_NUMBER, AC71_ERROR_LEXICAL, "AC71_ERR_LEX_INVALID_NUMBER",  "Invalid numeric literal." },
    { AC71_ERR_SYN_UNEXPECTED_TOKEN, AC71_ERROR_SYNTAX, "AC71_ERR_SYN_UNEXPECTED_TOKEN","Unexpected token found." },
    { AC71_ERR_SYN_MISSING_SEMICOLON, AC71_ERROR_SYNTAX, "AC71_ERR_SYN_MISSING_SEMICOLON", "Missing semicolon." },
    { AC71_ERR_SYN_UNMATCHED_BRACKET, AC71_ERROR_SYNTAX, "AC71_ERR_SYN_UNMATCHED_BRACKET", "Unmatched bracket." },
    { AC71_ERR_SYN_EXPECTED_OPEN_PAREN, AC71_ERROR_SYNTAX, "AC71_ERR_SYN_EXPECTED_OPEN_PAREN", "Expected '('." },
    { AC71_ERR_SYN_EXPECTED_CLOSE_PAREN, AC71_ERROR_SYNTAX, "AC71_ERR_SYN_EXPECTED_CLOSE_PAREN", "Expected ')'." },
    { AC71_ERR_SEM_UNDECLARED_IDENTIFIER, AC71_ERROR_SEMANTIC, "AC71_ERR_SEM_UNDECLARED_IDENTIFIER", "Undeclared identifier used." },
    { AC71_ERR_SEM_REDECLARATION, AC71_ERROR_SEMANTIC, "AC71_ERR_SEM_REDECLARATION", "Variable redeclared." },
    { AC71_ERR_SEM_TYPE_MISMATCH, AC71_ERROR_SEMANTIC, "AC71_ERR_SEM_TYPE_MISMATCH", "Type mismatch." },
    { AC71_ERR_SEM_INVALID_RETURN, AC71_ERROR_SEMANTIC, "AC71_ERR_SEM_INVALID_RETURN", "Invalid return statement." },
    { AC71_ERR_FATAL_OUT_OF_MEMORY, AC71_ERROR_FATAL, "AC71_ERR_FATAL_OUT_OF_MEMORY", "Out of memory." },
    { AC71_ERR_FATAL_INTERNAL, AC71_ERROR_FATAL, "AC71_ERR_FATAL_INTERNAL", "Internal compiler error." }
};

/////////////////////////////////////////////////////////////////////////////

const AC71_ErrorEntry* AC71_GetErrorEntry(AC71_ErrorCode code) {
    if (code >= 0 && code < AC71_ERR_COUNT) {
        return &AC71_ErrorTable[code];
    }
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////

const char* AC71_ErrorTypeToString(AC71_ErrorType type) {
    switch (type) {
        case AC71_ERROR_INFO: return "Info";
        case AC71_ERROR_WARNING: return "Warning";
        case AC71_ERROR_LEXICAL: return "Lexical error";
        case AC71_ERROR_SYNTAX: return "Syntax error";
        case AC71_ERROR_SEMANTIC: return "Semantic error";
        case AC71_ERROR_FATAL: return "Fatal error";
        default: return "Unknown error type";
    }
}

/////////////////////////////////////////////////////////////////////////////

void AC71_ReportError(AC71_ErrorCode code, int errorLine, int errorColumn) {
    const AC71_ErrorEntry* entry = AC71_GetErrorEntry(code);

    if (!entry) {
        fprintf(stderr, "[error][LINE %d, COLUMN %d] Unknown error code: %d\n", errorLine, errorColumn, code);
        return;
    }

    const char* errorType = AC71_ErrorTypeToString(entry->type);

    fprintf(stderr, "Error at line %d, column %d. %s: %s\n", errorLine, errorColumn, errorType, entry->message);

    if (entry->type == AC71_ERROR_FATAL) {
        exit(EXIT_FAILURE);
    }
}
