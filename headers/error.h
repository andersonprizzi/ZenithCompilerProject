#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

/////////////////////////////////////////////////////////////////////////////
// DEFINITION OF TYPES                                                     //
/////////////////////////////////////////////////////////////////////////////

typedef enum {
    // Informativos e avisos
    AC71_ERR_INFO_SUCCESS,
    AC71_ERR_WARNING_UNUSED_VAR,

    // Erros léxicos
    AC71_ERR_LEX_INVALID_CHAR,
    AC71_ERR_LEX_UNCLOSED_STRING,
    AC71_ERR_LEX_INVALID_NUMBER,

    // Erros sintáticos
    AC71_ERR_SYN_UNEXPECTED_TOKEN,
    AC71_ERR_SYN_MISSING_SEMICOLON,
    AC71_ERR_SYN_UNMATCHED_BRACKET,
    AC71_ERR_SYN_EXPECTED_OPEN_PAREN,
    AC71_ERR_SYN_EXPECTED_CLOSE_PAREN,

    // Erros semânticos
    AC71_ERR_SEM_UNDECLARED_IDENTIFIER,
    AC71_ERR_SEM_REDECLARATION,
    AC71_ERR_SEM_TYPE_MISMATCH,
    AC71_ERR_SEM_INVALID_RETURN,

    // Erros fatais
    AC71_ERR_FATAL_OUT_OF_MEMORY,
    AC71_ERR_FATAL_INTERNAL,

    // Total de erros definidos
    AC71_ERR_COUNT
} AC71_ErrorCode;


typedef enum {
    AC71_ERROR_INFO,
    AC71_ERROR_WARNING,
    AC71_ERROR_LEXICAL,
    AC71_ERROR_SYNTAX,
    AC71_ERROR_SEMANTIC,
    AC71_ERROR_FATAL
} AC71_ErrorType;


typedef struct {
    AC71_ErrorCode code;
    AC71_ErrorType type;
    const char* name;
    const char* message;
} AC71_ErrorEntry;



/////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES                                                     //
/////////////////////////////////////////////////////////////////////////////

void AC71_ReportError(AC71_ErrorCode code, int errorLine, int errorColumn);
const AC71_ErrorEntry* AC71_GetErrorEntry(AC71_ErrorCode code);
const char* AC71_ErrorTypeToString(AC71_ErrorType type);

/////////////////////////////////////////////////////////////////////////////

#endif // ERROR_H_INCLUDED
