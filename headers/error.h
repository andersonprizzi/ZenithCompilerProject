#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

/////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS                                                        //
/////////////////////////////////////////////////////////////////////////////

typedef enum {
    CC71_ERR_LEX_INVALID_CHAR,
    CC71_ERR_LEX_UNCLOSED_STRING,
    CC71_ERR_LEX_INVALID_NUMBER,

    CC71_ERR_SYN_UNEXPECTED_TOKEN,
    CC71_ERR_SYN_MISSING_SEMICOLON,
    CC71_ERR_SYN_UNMATCHED_BRACKET,
    CC71_ERR_SYN_EXPECTED_OPEN_PAREN,
    CC71_ERR_SYN_EXPECTED_CLOSE_PAREN,

    CC71_ERR_SEM_UNDECLARED_IDENTIFIER,
    CC71_ERR_SEM_REDECLARATION,
    CC71_ERR_SEM_TYPE_MISMATCH,
    CC71_ERR_SEM_INVALID_RETURN,

    CC71_ERR_FATAL_OUT_OF_MEMORY,
    CC71_ERR_FATAL_INTERNAL,

    CC71_ERR_COUNT
} CC71_ErrorCode;


typedef enum {
    CC71_ERROR_LEXICAL,
    CC71_ERROR_SYNTAX,
    CC71_ERROR_SEMANTIC,
    CC71_ERROR_FATAL
} CC71_ErrorType;


typedef struct {
    CC71_ErrorCode code;
    CC71_ErrorType type;
    const char* name;
    const char* message;
} CC71_ErrorEntry;



/////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES                                                     //
/////////////////////////////////////////////////////////////////////////////

int CC71_ReportError(CC71_ErrorCode code, int line, int column, const char* fmt, ...);
const CC71_ErrorEntry* CC71_GetErrorEntry(CC71_ErrorCode code);
const char* CC71_GetErrorType(CC71_ErrorType type);
const char* CC71_TokenToString(int token);

#endif // ERROR_H_INCLUDED
