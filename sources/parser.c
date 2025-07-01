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

#define BACKTRACK_START() CC71_BacktrackingStart()
#define BACKTRACKING_RESTORE() CC71_BacktrackingRestore()
#define BACKTRACK_FAIL() CC71_BacktrackingRestore(); return 0
#define BACKTRACK_END()   CC71_BacktrackingEnd()
#define PARSE_FAIL 0
#define PARSE_SUCCESS 1



/////////////////////////////////////////////////////////////////////////////
// FUNCTION IMPLEMENTATIONS                                                //
/////////////////////////////////////////////////////////////////////////////

// OK
int translation_unit() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "translation_unit()");
    
    if (!external_declaration()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "translation_unit()");
        return PARSE_FAIL;
    }

    if (!translation_unit_sequence()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "translation_unit()");
        return PARSE_FAIL;
    }

    if (CC71_GlobalTokenNumber != TokenEndOfFile) {
        CC71_ReportError(CC71_ERR_SYN_EXPECTED_EOF, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, "Expected end of file, but got '%s'", CC71_TokenToString(CC71_GlobalTokenNumber));
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "translation_unit()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "translation_unit()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int translation_unit_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "translation_unit_sequence()");
    
    if (CC71_GlobalTokenNumber == TokenEndOfFile) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "translation_unit_sequence() (EOF)");
        return PARSE_SUCCESS;
    }

    if (external_declaration()) {
        if (translation_unit_sequence()) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "translation_unit_sequence()");
            return PARSE_SUCCESS;
        }
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "translation_unit_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int external_declaration() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "external_declaration()");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));

    CC71_BacktrackingStart();

    if (function_definition()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "external_declaration()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingRestore();
    CC71_BacktrackingStart();

    if (external_definition()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "external_declaration()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingRestore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "external_declaration()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int function_definition() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "function_definition()");
    
    if (!declaration_specifiers()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "function_definition()");
        return PARSE_FAIL;
    }

    if (!declarator()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "function_definition()");
        return PARSE_FAIL;
    }

    if (!compound_statement()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "function_definition()");
        return CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, "Expected compound statement in function definition.");
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "function_definition()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int external_definition() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "external_definition()");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));
    
    if (!declaration_specifiers()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "external_definition()");
        return PARSE_FAIL;
    }

    if (declaration_suffix()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "external_definition()");
        return PARSE_SUCCESS;
    } else {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "external_definition()");
        return PARSE_FAIL;
    }
}


/////////////////////////////////////////////////////////////////////////////


// OK
int declaration_specifiers() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "declaration_specifiers()");
    
    if (!declaration_specifier()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "declaration_specifiers()");
        return PARSE_FAIL;
    }

    declaration_specifiers_sequence(); 
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "declaration_specifiers()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int declaration_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "declaration_specifier()");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));

    if (storage_specifier()) { 
        return PARSE_SUCCESS; 
    } else if (type_specifier()) { 
        return PARSE_SUCCESS; 
    } else if (type_qualifier()) { 
        return PARSE_SUCCESS; 
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "declaration_specifier()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int declaration_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "declaration_suffix()");

    CC71_BacktrackingStart();

    if (!init_declarator()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "declaration_suffix()");
        return PARSE_FAIL;
    }

    while (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!init_declarator()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "declaration_suffix()");
            return PARSE_FAIL;
        }
    }

    if (CC71_GlobalTokenNumber != TokenSemicolon) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "declaration_suffix()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    CC71_GetToken();
    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "declaration_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int declaration_specifiers_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "declaration_specifiers_sequence()");

    if (declaration_specifier()) {
        if (declaration_specifiers_sequence()) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "declaration_specifiers_sequence()");
            return PARSE_SUCCESS;
        }
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "declaration_specifiers_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int declaration_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "declaration_list()");
    
    if (!external_definition()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "declaration_list()");
        return PARSE_FAIL;
    }

    if (!declaration_list_sequence()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "declaration_list()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "declaration_list()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int declaration_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "declaration_list_sequence()");

    CC71_BacktrackingStart();

    if (external_definition()) {
        if (!declaration_list_sequence()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "declaration_list_sequence()");
            return PARSE_FAIL;
        }
        
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "declaration_list_sequence()");
        return PARSE_SUCCESS;
    }
    
    CC71_BacktrackingRestore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "declaration_list_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int init_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "init_declarator()");

    if (!declarator()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "init_declarator()");
        return PARSE_FAIL;
    }

    if (!init_declarator_sequence()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "init_declarator()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "init_declarator()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int init_declarator_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "init_declarator_sequence()");

    if (CC71_GlobalTokenNumber == TokenAssign) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!initializer()) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "init_declarator_sequence()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "init_declarator_sequence()");
        return PARSE_SUCCESS;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "init_declarator_sequence() (epsilon)");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "declarator()");

    if (pointer_declarator()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "declarator()");
        return PARSE_SUCCESS;
    }

    if (direct_declarator()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "declarator()");
        return PARSE_SUCCESS;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "declarator()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int pointer_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "pointer_declarator()");

    CC71_BacktrackingStart();

    if (!pointer()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "pointer_declarator()");
        CC71_BacktrackingRestore();
        return PARSE_FAIL;
    }

    if (!direct_declarator()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "pointer_declarator()");
        CC71_BacktrackingRestore();
        return PARSE_FAIL;
    }

    CC71_BacktrackingEnd();

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "pointer_declarator()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int direct_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "direct_declarator()");

    CC71_BacktrackingStart();

    if (!direct_declarator_base()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator()");
        return PARSE_FAIL;
    }

    if (!direct_declarator_sequence()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator()");

        return PARSE_FAIL;
    }

    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int direct_declarator_base() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "direct_declarator_base()");
    
    CC71_BacktrackingStart();

    if (CC71_GlobalTokenNumber == TokenIdentifier) {
        printf("PASSOU AQUI 1\n");
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_base()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        
        if (!declarator()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_base()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_BacktrackingRestore();
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_base()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_base()");
        CC71_BacktrackingEnd();
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingRestore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_base()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int direct_declarator_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "direct_declarator_sequence()");

    CC71_BacktrackingStart();

    if (direct_declarator_suffix()) {
        if (!direct_declarator_sequence()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_sequence()");
            return PARSE_FAIL;
        }

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_sequence()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int direct_declarator_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "direct_declarator_suffix()");

    CC71_BacktrackingStart();

    switch (CC71_GlobalTokenNumber) {
        case TokenOpenBracket:
            printf("PASSOU AQUI 2\n");
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();

            if (CC71_GlobalTokenNumber == TokenCloseBracket) {
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                CC71_GetToken();
                CC71_BacktrackingEnd();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_suffix()");
                return PARSE_SUCCESS;
            }

            if (!constant_expression()) {
                CC71_BacktrackingRestore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_suffix()");
                return PARSE_FAIL;
            }

            if (CC71_GlobalTokenNumber != TokenCloseBracket) {
                printf("PASSOU AQUI 3\n");
                CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                CC71_BacktrackingRestore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_suffix()");
                return PARSE_FAIL;
            }

            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_BacktrackingEnd();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_suffix()");
            return PARSE_SUCCESS;

        case TokenOpenParentheses:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();

            if (parameter_type_list()) {
                if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                    CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                    CC71_BacktrackingRestore();
                    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_suffix()");
                    return PARSE_FAIL;
                }

                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                CC71_GetToken();
                CC71_BacktrackingEnd();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_suffix()");
                return PARSE_SUCCESS;

            } else if (identifier_list()) {
                if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                    CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                    CC71_BacktrackingRestore();
                    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_suffix()");
                    return PARSE_FAIL;
                }

                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                CC71_GetToken();
                CC71_BacktrackingEnd();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_suffix()");
                return PARSE_SUCCESS;

            } else if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                CC71_GetToken();
                CC71_BacktrackingEnd();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_suffix()");
                return PARSE_SUCCESS;
            }

            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_suffix()");
            return PARSE_FAIL;

        default:
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_suffix()");
            return PARSE_FAIL;
    }
}


/////////////////////////////////////////////////////////////////////////////


// OK
int abstract_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "abstract_declarator()");

    CC71_BacktrackingStart();

    if (pointer()) {
        if (!abstract_declarator_sequence()) BACKTRACK_FAIL();
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "abstract_declarator()");
        return PARSE_SUCCESS;
    }

    if (direct_abstract_declarator()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "abstract_declarator()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingRestore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "abstract_declarator()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int abstract_declarator_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "abstract_declarator_sequence()");

    CC71_BacktrackingStart();

    if (direct_abstract_declarator()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "abstract_declarator_sequence()");
        return PARSE_SUCCESS;
    }
    
    CC71_BacktrackingEnd();

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "abstract_declarator_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int direct_abstract_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "direct_abstract_declarator()");

    CC71_BacktrackingStart();

    if (!direct_abstract_declarator_base()) BACKTRACK_FAIL();
    if (!direct_abstract_declarator_sequence()) BACKTRACK_FAIL();

    CC71_BacktrackingEnd();

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_abstract_declarator()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int direct_abstract_declarator_base() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "direct_abstract_declarator_base()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (!abstract_declarator()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenOpenBracket) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (CC71_GlobalTokenNumber == TokenCloseBracket) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        }
        if (!constant_expression()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseBracket) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (parameter_type_list()) {
            if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                BACKTRACK_FAIL();
            }
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        } else if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        }
        BACKTRACK_FAIL();
    }
    BACKTRACK_FAIL();
}


/////////////////////////////////////////////////////////////////////////////


int direct_abstract_declarator_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the direct_abstract_declarator_sequence() function.");

    BACKTRACK_START();
    if (direct_abstract_declarator_suffix()) {
        if (!direct_abstract_declarator_sequence()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int direct_abstract_declarator_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the direct_abstract_declarator_suffix() function.");

    BACKTRACK_START();
    switch(CC71_GlobalTokenNumber) {
        case TokenOpenBracket:
            CC71_GetToken();
            if (CC71_GlobalTokenNumber == TokenCloseBracket) {
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                CC71_GetToken();
                BACKTRACK_END();
                return 1;
            }
            if (!constant_expression()) BACKTRACK_FAIL();
            if (CC71_GlobalTokenNumber != TokenCloseBracket) {
                CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                BACKTRACK_FAIL();
            }
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        case TokenOpenParentheses:
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                CC71_GetToken();
                BACKTRACK_END();
                return 1;
            }
            if (parameter_type_list()) {
                if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                    CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                    BACKTRACK_FAIL();
                }
                CC71_GetToken();
                BACKTRACK_END();
                return 1;
            }
            BACKTRACK_FAIL();
        default:
            BACKTRACK_FAIL();
    }
}


/////////////////////////////////////////////////////////////////////////////


// OK
int pointer() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "pointer()");

    CC71_BacktrackingStart();

    if (CC71_GlobalTokenNumber != TokenAsterisk) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "pointer()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    CC71_GetToken();

    if (pointer_suffix()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "pointer()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingRestore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "pointer()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


int pointer_suffix() {
    BACKTRACK_START();
    if (type_qualifier_list()) {
        if (pointer()) {
            BACKTRACK_END();
            return 1;
        }
        BACKTRACK_END();
        return 1;
    }
    if (pointer()) {
        BACKTRACK_END();
        return 1;
    }
    
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int parameter_type_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "parameter_type_list()");

    CC71_BacktrackingStart();

    if (!parameter_list()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_type_list()");
        CC71_BacktrackingRestore();
        return PARSE_FAIL;
    }

    if (!parameter_type_sequence()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_type_list()");
        CC71_BacktrackingRestore();
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_type_list()");
    CC71_BacktrackingEnd();
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int parameter_type_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "parameter_type_sequence()");

    CC71_BacktrackingStart();

    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (CC71_GlobalTokenNumber == TokenEllipsis) {
            CC71_GetToken();
            CC71_BacktrackingEnd();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_type_sequence()");
            return PARSE_SUCCESS;
        } else {
            CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_type_sequence()");
            return PARSE_FAIL;
        }
    }
    
    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_type_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int parameter_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "parameter_list()");

    CC71_BacktrackingStart();

    if (!parameter_declaration()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_list()");
        return PARSE_FAIL;
    }

    if (!parameter_sequence()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_list()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_list()");
    CC71_BacktrackingEnd();
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int parameter_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "parameter_sequence()");

    CC71_BacktrackingStart();

    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!parameter_declaration()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_sequence()");
            return PARSE_FAIL;
        }

        if (!parameter_sequence()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_sequence()");
            return PARSE_FAIL;
        }

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_sequence()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int parameter_declaration() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "parameter_declaration()");

    CC71_BacktrackingStart();

    if (!declaration_specifiers()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_declaration()");
        return PARSE_FAIL;
    }

    if (!parameter_declaration_suffix()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_declaration()");
        return PARSE_FAIL;
    }

    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_declaration()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int parameter_declaration_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "parameter_declaration_suffix()");

    CC71_BacktrackingStart();

    if (declarator()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_declaration_suffix()");
        return PARSE_SUCCESS;
    }

    if (abstract_declarator()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_declaration_suffix()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_declaration_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int identifier_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "identifier_list()");

    CC71_BacktrackingStart();

    if (CC71_GlobalTokenNumber != TokenIdentifier) {
        CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "identifier_list()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    CC71_GetToken();

    if (!identifier_list_sequence()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "identifier_list()");
        return PARSE_FAIL;
    }

    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "identifier_list()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int identifier_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "identifier_list_sequence()");

    CC71_BacktrackingStart();

    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (CC71_GlobalTokenNumber != TokenIdentifier) {
            CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "identifier_list_sequence()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!identifier_list_sequence()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "identifier_list_sequence()");
            return PARSE_FAIL;
        }

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "identifier_list_sequence()");
        return PARSE_SUCCESS;
    }
    
    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "identifier_list_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int initializer() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "initializer()");

    CC71_BacktrackingStart();

    if (assignment_expression()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "initializer()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenOpenBrace) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!initializer_list()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer()");
            return PARSE_FAIL;
        }

        if (!initializer_suffix()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer()");
            return PARSE_FAIL;
        }

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "initializer()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingRestore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int initializer_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "initializer_suffix()");

    CC71_BacktrackingStart();

    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (CC71_GlobalTokenNumber != TokenCloseBrace) {
            CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer_suffix()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "initializer_suffix()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenCloseBrace) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "initializer_suffix()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingRestore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer_suffix()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


int initializer_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "initializer_list()");

    BACKTRACK_START();
    if (!initializer()) BACKTRACK_FAIL();
    if (!initializer_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int initializer_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "initializer_sequence()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (!initializer()) BACKTRACK_FAIL();
        if (!initializer_sequence()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////


int struct_or_union_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_or_union_specifier()");

    if (CC71_GlobalTokenNumber == TokenStruct || CC71_GlobalTokenNumber == TokenUnion) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!struct_or_union_body()) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_or_union_specifier()");
            return PARSE_FAIL;
        }
        
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "struct_or_union_specifier()");
        return PARSE_SUCCESS;
    }
    
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_or_union_specifier()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


int struct_or_union_body() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_or_union_body()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenIdentifier) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (CC71_GlobalTokenNumber == TokenOpenBrace) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            if (!field_declaration_list()) BACKTRACK_FAIL();
            if (CC71_GlobalTokenNumber != TokenCloseBrace) {
                CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                BACKTRACK_FAIL();
            }

            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            BACKTRACK_END();

            return 1;
        }
        // Apenas TokenIdentifier
        BACKTRACK_END();
        return 1;
    }

    if (CC71_GlobalTokenNumber == TokenOpenBrace) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!field_declaration_list()) {
            BACKTRACK_FAIL();
        }

        if (CC71_GlobalTokenNumber != TokenCloseBrace) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }

    BACKTRACK_FAIL();
}


/////////////////////////////////////////////////////////////////////////////


int field_declaration_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "field_declaration_list()");

    BACKTRACK_START();
    if (!struct_declaration()) BACKTRACK_FAIL();
    if (!field_declaration_list_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int field_declaration_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "field_declaration_list_sequence()");

    BACKTRACK_START();
    if (struct_declaration()) {
        if (!field_declaration_list_sequence()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int struct_declaration() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_declaration()");

    BACKTRACK_START();

    if (!specifier_qualifier_list()) {
        BACKTRACK_FAIL();
    }

    if (!struct_declarator_list()) {
        BACKTRACK_FAIL();
    }

    if (CC71_GlobalTokenNumber != TokenSemicolon) {
        CC71_ReportError(CC71_ERR_SYN_MISSING_SEMICOLON, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
        BACKTRACK_FAIL();
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    CC71_GetToken();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int struct_declarator_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_declarator_list()");

    BACKTRACK_START();
    if (!struct_declarator()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Failed to parse struct_declarator in struct_declarator_list()");
        BACKTRACK_FAIL();
    }
    if (!struct_declarator_list_sequence()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Failed to parse struct_declarator_list_sequence in struct_declarator_list()");
        BACKTRACK_FAIL();
    }
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int struct_declarator_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_declarator_list_sequence()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!struct_declarator()) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Failed to parse struct_declarator after ',' in struct_declarator_list_sequence()");
            BACKTRACK_FAIL();
        }

        if (!struct_declarator_list_sequence()) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Failed to parse struct_declarator_list_sequence recursively");
            BACKTRACK_FAIL();
        }

        BACKTRACK_END();
        return 1;
    }
    // ε (vazio)
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int struct_declarator_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_declarator_suffix()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenColon) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (!constant_expression()) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Expected constant_expression after ':' in struct_declarator_suffix()");
            BACKTRACK_FAIL();
        }
        BACKTRACK_END();
        return 1;
    }
    // epsilon (vazio)
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int struct_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_declarator()");

    BACKTRACK_START();

    if (CC71_GlobalTokenNumber == TokenColon) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!constant_expression()) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Expected constant_expression after ':' in struct_declarator()");
            BACKTRACK_FAIL();
        }

        BACKTRACK_END();
        return 1;
    } else {
        // Regra: declarator struct_declarator_suffix
        if (!declarator()) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Expected declarator in struct_declarator()");
            BACKTRACK_FAIL();
        }
        if (!struct_declarator_suffix()) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Failed parsing struct_declarator_suffix()");
            BACKTRACK_FAIL();
        }
        BACKTRACK_END();
        return 1;
    }

    BACKTRACK_FAIL(); // Não deve chegar aqui, mas para garantir
}


/////////////////////////////////////////////////////////////////////////////


int enumerator_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "enumerator_specifier()");

    if (CC71_GlobalTokenNumber == TokenEnum) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (!enumerator_body()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }

    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


int enumerator_body() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "enumerator_body()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenOpenBrace) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (!enumerator_list()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseBrace) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }

    if (CC71_GlobalTokenNumber == TokenIdentifier) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (CC71_GlobalTokenNumber == TokenOpenBrace) {
            CC71_GetToken();
            if (!enumerator_list()) BACKTRACK_FAIL();

            if (CC71_GlobalTokenNumber != TokenCloseBrace) {
                CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                BACKTRACK_FAIL();
            }

            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        }
        BACKTRACK_FAIL();
    }
    BACKTRACK_FAIL();
}


/////////////////////////////////////////////////////////////////////////////


int enumerator_list() {
    BACKTRACK_START();
    if (!enumerator()) BACKTRACK_FAIL();
    if (!enumerator_list_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int enumerator_list_sequence() {
    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!enumerator()) {
            BACKTRACK_FAIL();
        }

        if (!enumerator_list_sequence()) {
            BACKTRACK_FAIL();
        }

        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int enumerator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "enumerator()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber != TokenIdentifier) {
        CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
        BACKTRACK_FAIL();
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    CC71_GetToken();
    if (!enumerator_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int enumerator_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "enumerator_suffix()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenAssign) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (!constant_expression()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int compound_statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "compound_statement()");

    CC71_BacktrackingStart();

    if (CC71_GlobalTokenNumber != TokenOpenBrace) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "compound_statement()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    CC71_GetToken();

    if (CC71_GlobalTokenNumber == TokenCloseBrace) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "compound_statement()");
        return PARSE_SUCCESS;
    }

    if (declaration_list()) {
        if (statement_list()) {
            if (CC71_GlobalTokenNumber == TokenCloseBrace) {
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                CC71_GetToken();
                CC71_BacktrackingEnd();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "compound_statement()");
                return PARSE_SUCCESS;
            }
            
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "compound_statement()");
            return PARSE_FAIL;
        }
        
        if (CC71_GlobalTokenNumber == TokenCloseBrace) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_BacktrackingEnd();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "compound_statement()");
            return PARSE_SUCCESS;
        }

        CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "compound_statement()");
        return PARSE_FAIL;
    }

    if (statement_list()) {
        if (CC71_GlobalTokenNumber == TokenCloseBrace) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_BacktrackingEnd();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "compound_statement()");
            return PARSE_SUCCESS;
        }

        CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "compound_statement()");
        return PARSE_FAIL;
    }

    CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
    CC71_BacktrackingRestore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "compound_statement()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int statement_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "statement_list()");

    CC71_BacktrackingStart();

    if (!statement()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "statement_list()");
        return PARSE_FAIL;
    }

    if (!statement_list_sequence()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "statement_list()");
        return PARSE_FAIL;
    }

    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement_list()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int statement_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "statement_list_sequence()");

    CC71_BacktrackingStart();
    
    if (statement()) {
        if (!statement_list_sequence()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "statement_list_sequence()");
            return PARSE_FAIL;
        }

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement_list_sequence()");
        return PARSE_SUCCESS;
    }
    
    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement_list_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "statement()");

    CC71_BacktrackingStart();

    if (labeled_statement()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement()");
        return PARSE_SUCCESS;
    }
    if (compound_statement()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement()");
        return PARSE_SUCCESS;
    }
    if (expression_statement()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement()");
        return PARSE_SUCCESS;
    }
    if (selection_statement()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement()");
        return PARSE_SUCCESS;
    }
    if (iteration_statement()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement()");
        return PARSE_SUCCESS;
    }
    if (jump_statement()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingRestore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "statement()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


int labeled_statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "labeled_statement()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenIdentifier) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (CC71_GlobalTokenNumber != TokenColon) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!statement()) {
            BACKTRACKING_RESTORE();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "labeled_statement()");
            return PARSE_FAIL;
        }
        
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "labeled_statement()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenCase) {
        CC71_GetToken();
        if (!constant_expression()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenColon) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        if (!statement()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenDefault) {
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenColon) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        if (!statement()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    BACKTRACK_FAIL();
}


/////////////////////////////////////////////////////////////////////////////


int expression_statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "expression_statement()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenSemicolon) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "expression_statement()");
        return PARSE_SUCCESS;
    }

    if (expression()) {
        if (CC71_GlobalTokenNumber == TokenSemicolon) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "expression_statement()");
            return PARSE_SUCCESS;
        }

        CC71_ReportError(CC71_ERR_SYN_MISSING_SEMICOLON, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
        BACKTRACKING_RESTORE();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "expression_statement()");
        return PARSE_FAIL;
    }

    BACKTRACKING_RESTORE();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "expression_statement()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


int selection_statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "selection_statement()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenIf) {
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenOpenParentheses) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_OPEN_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        if (!expression()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        if (!statement()) BACKTRACK_FAIL();
        if (!selection_statement_else_opt()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenSwitch) {
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenOpenParentheses) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_OPEN_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        if (!expression()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        if (!statement()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    BACKTRACK_FAIL();
}


/////////////////////////////////////////////////////////////////////////////


int selection_statement_else_opt() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "selection_statement_else_opt()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenElse) {
        CC71_GetToken();
        if (!statement()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int iteration_statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "iteration_statement()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenWhile) {
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenOpenParentheses) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_OPEN_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        if (!expression()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        if (!statement()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenDo) {
        CC71_GetToken();
        if (!statement()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenWhile) {
            CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenOpenParentheses) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_OPEN_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        if (!expression()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenSemicolon) {
            CC71_ReportError(CC71_ERR_SYN_MISSING_SEMICOLON, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenFor) {
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenOpenParentheses) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_OPEN_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        if (!expression_statement()) BACKTRACK_FAIL();
        if (!expression_statement()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
            CC71_GetToken();
            if (!statement()) BACKTRACK_FAIL();
            BACKTRACK_END();
            return 1;
        }
        if (!expression()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        if (!statement()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    BACKTRACK_FAIL();
}


/////////////////////////////////////////////////////////////////////////////


int jump_statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "jump_statement()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenGoto) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (CC71_GlobalTokenNumber != TokenIdentifier) {
            CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (CC71_GlobalTokenNumber != TokenSemicolon) {
            CC71_ReportError(CC71_ERR_SYN_MISSING_SEMICOLON, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }

    if (CC71_GlobalTokenNumber == TokenContinue) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (CC71_GlobalTokenNumber != TokenSemicolon) {
            CC71_ReportError(CC71_ERR_SYN_MISSING_SEMICOLON, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }

    if (CC71_GlobalTokenNumber == TokenBreak) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenSemicolon) {
            CC71_ReportError(CC71_ERR_SYN_MISSING_SEMICOLON, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }

    if (CC71_GlobalTokenNumber == TokenReturn) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (CC71_GlobalTokenNumber == TokenSemicolon) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "jump_statement()");
            return PARSE_SUCCESS;
        }

        if (expression()) {
            if (CC71_GlobalTokenNumber != TokenSemicolon) {
                CC71_ReportError(CC71_ERR_SYN_MISSING_SEMICOLON, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                BACKTRACKING_RESTORE();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "jump_statement()");
                return PARSE_FAIL;
            }
            
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "jump_statement()");
            return PARSE_SUCCESS;
        }

        CC71_ReportError(CC71_ERR_SYN_MISSING_SEMICOLON, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
        BACKTRACK_FAIL();
    }

    BACKTRACK_FAIL();
}


/////////////////////////////////////////////////////////////////////////////


// OK
int expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "expression()");

    CC71_BacktrackingStart();

    if (!assignment_expression()) {
        CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, "Expected assignment_expression in expression.");
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "expression()");
        return PARSE_FAIL;
    }

    if (!expression_sequence()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "expression()");
        return PARSE_FAIL;
    }

    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int expression_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "expression_sequence()");

    CC71_BacktrackingStart();

    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!assignment_expression()) {
            CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, "Expected assignment_expression after ','.");
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "expression_sequence()");
            return PARSE_FAIL;
        }

        if (!expression_sequence()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "expression_sequence()");
            return PARSE_FAIL;
        }

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "expression_sequence()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "expression_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int constant_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "constant_expression()");

    if (!conditional_expression()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "constant_expression()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "constant_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int argument_expression_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "argument_expression_list()");

    BACKTRACK_START();
    if (!assignment_expression()) BACKTRACK_FAIL();
    if (!argument_expression_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int argument_expression_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "argument_expression_sequence()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_GetToken();
        if (!assignment_expression()) BACKTRACK_FAIL();
        if (!argument_expression_sequence()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int assignment_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "assignment_expression()");

    CC71_BacktrackingStart();

    if (unary_expression()) {
        if (assignment_operator()) {
            if (assignment_expression()) {
                CC71_BacktrackingEnd();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "assignment_expression()");
                return PARSE_SUCCESS;
            }
        }
    }
    
    CC71_BacktrackingRestore();
    //CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "RESTAURADO Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));
    CC71_BacktrackingStart();

    if (conditional_expression()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "assignment_expression()");
        return PARSE_SUCCESS;
    }
    
    CC71_BacktrackingRestore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "assignment_expression()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


int conditional_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "conditional_expression()");

    CC71_BacktrackingStart();

    if (!logical_or_expression()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "conditional_expression()");     
        return PARSE_FAIL;
    }

    if (!conditional_expression_suffix()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "conditional_expression()");
        return PARSE_FAIL;
    }
    
    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "conditional_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int conditional_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "conditional_expression_suffix()");

    CC71_BacktrackingStart();
    
    if (CC71_GlobalTokenNumber == TokenQuestionMark) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!expression()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "conditional_expression_suffix()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenColon) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "conditional_expression_suffix()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!conditional_expression()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "conditional_expression_suffix()");
            return PARSE_FAIL;
        }

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "conditional_expression_suffix()");
        return PARSE_SUCCESS;
    }
    
    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "conditional_expression_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int logical_or_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "logical_or_expression()");

    if (!logical_and_expression()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "logical_or_expression()");
        return PARSE_FAIL;
    }

    if (!logical_or_expression_suffix()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "logical_or_expression()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "logical_or_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int logical_or_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "logical_or_expression_suffix()");

    if (CC71_GlobalTokenNumber == TokenLogicalOr) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!logical_and_expression()) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "logical_or_expression_suffix()");
            return PARSE_FAIL;
        }

        if (!logical_or_expression_suffix()) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "logical_or_expression_suffix()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "logical_or_expression_suffix()");
        return PARSE_SUCCESS;
    }
    
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "logical_or_expression_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int logical_and_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "logical_and_expression()");
    
    if (!inclusive_or_expression()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "logical_and_expression()");
        return PARSE_FAIL;
    }

    if (!logical_and_expression_suffix()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "logical_and_expression()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "logical_and_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int logical_and_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "logical_and_expression_suffix()");
    
    CC71_BacktrackingStart();

    if (CC71_GlobalTokenNumber == TokenLogicalAnd) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!inclusive_or_expression()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "logical_and_expression_suffix()");
            return PARSE_FAIL;
        }

        if (!logical_and_expression_suffix()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "logical_and_expression_suffix()");
            return PARSE_FAIL;
        }

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "logical_and_expression_suffix()");
        return PARSE_SUCCESS;
    }
    
    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "logical_and_expression_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int inclusive_or_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "inclusive_or_expression()");

    BACKTRACK_START();
    if (!exclusive_or_expression()) BACKTRACK_FAIL();
    if (!inclusive_or_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int inclusive_or_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "inclusive_or_expression_suffix()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenBitwiseOr) {
        CC71_GetToken();
        if (!exclusive_or_expression()) BACKTRACK_FAIL();
        if (!inclusive_or_expression_suffix()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int exclusive_or_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "exclusive_or_expression()");

    BACKTRACK_START();
    if (!and_expression()) BACKTRACK_FAIL();
    if (!exclusive_or_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int exclusive_or_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "exclusive_or_expression_suffix()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenBitwiseXor) {
        CC71_GetToken();
        if (!and_expression()) BACKTRACK_FAIL();
        if (!exclusive_or_expression_suffix()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int and_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "and_expression()");

    BACKTRACK_START();
    if (!equality_expression()) BACKTRACK_FAIL();
    if (!and_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int and_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "and_expression_suffix()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenBitwiseAnd_AddressOf) {
        CC71_GetToken();
        if (!equality_expression()) BACKTRACK_FAIL();
        if (!and_expression_suffix()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int equality_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "equality_expression()");

    BACKTRACK_START();
    if (!relational_expression()) BACKTRACK_FAIL();
    if (!equality_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int equality_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "equality_expression_suffix()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenEqual || CC71_GlobalTokenNumber == TokenNotEqual) {
        CC71_GetToken();
        if (!relational_expression()) BACKTRACK_FAIL();
        if (!equality_expression_suffix()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int relational_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "relational_expression()");

    BACKTRACK_START();
    if (!shift_expression()) BACKTRACK_FAIL();
    if (!relational_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int relational_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "relational_expression_suffix()");

    BACKTRACK_START();
    switch (CC71_GlobalTokenNumber) {
        case TokenLessThan:
        case TokenGreaterThan:
        case TokenLessEqual:
        case TokenGreaterEqual:
            CC71_GetToken();
            if (!shift_expression()) BACKTRACK_FAIL();
            if (!relational_expression_suffix()) BACKTRACK_FAIL();
            BACKTRACK_END();
            return 1;
        default:
            BACKTRACK_END();
            return 1;
    }
}


/////////////////////////////////////////////////////////////////////////////


int shift_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "shift_expression()");

    BACKTRACK_START();
    if (!additive_expression()) BACKTRACK_FAIL();
    if (!shift_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int shift_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "shift_expression_suffix()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenLeftShift || CC71_GlobalTokenNumber == TokenRightShift) {
        CC71_GetToken();
        if (!additive_expression()) BACKTRACK_FAIL();
        if (!shift_expression_suffix()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int additive_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "additive_expression()");

    BACKTRACK_START();
    if (!multiplicative_expression()) BACKTRACK_FAIL();
    if (!additive_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int additive_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "additive_expression_suffix()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenPlus || CC71_GlobalTokenNumber == TokenMinus) {
        CC71_GetToken();
        if (!multiplicative_expression()) BACKTRACK_FAIL();
        if (!additive_expression_suffix()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int multiplicative_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "multiplicative_expression()");

    BACKTRACK_START();
    if (!cast_expression()) {
        BACKTRACK_FAIL();
    }

    if (!multiplicative_expression_suffix()) {
        BACKTRACK_FAIL();
    }

    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int multiplicative_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "multiplicative_expression_suffix()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenAsterisk || CC71_GlobalTokenNumber == TokenDivision || CC71_GlobalTokenNumber == TokenMod) {
        CC71_GetToken();
        if (!cast_expression()) BACKTRACK_FAIL();
        if (!multiplicative_expression_suffix()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int cast_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "cast_expression()");

    if (unary_expression()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "cast_expression()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingStart();

    if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!type_name()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "cast_expression()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "cast_expression()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!cast_expression()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "cast_expression()");
            return PARSE_FAIL;
        }

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "cast_expression()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingRestore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "cast_expression()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int unary_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "unary_expression()");

    CC71_BacktrackingStart();

    if (CC71_GlobalTokenNumber == TokenIncrement) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!unary_expression()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
            return PARSE_FAIL;
        }

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "unary_expression() (prefix ++)");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenDecrement) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!unary_expression()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
            return PARSE_FAIL;
        }

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "unary_expression() (prefix --)");
        return PARSE_SUCCESS;
    }

    if (unary_operator()) {
        if (!cast_expression()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
            return PARSE_FAIL;
        }

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "unary_expression() (unary_operator)");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenSizeof) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (unary_expression()) {
            CC71_BacktrackingEnd();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "unary_expression() (sizeof expr)");
            return PARSE_SUCCESS;
        }

        if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();

            if (!type_name()) {
                CC71_BacktrackingRestore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
                return PARSE_FAIL;
            }

            if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                CC71_BacktrackingRestore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
                return PARSE_FAIL;
            }

            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();

            CC71_BacktrackingEnd();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "unary_expression() (sizeof type)");
            return PARSE_SUCCESS;
        }

        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
        return PARSE_FAIL;
    }

    if (postfix_expression()) {
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "unary_expression() (postfix)");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingRestore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int postfix_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "postfix_expression()");

    CC71_BacktrackingStart();

    if (!primary_expression()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression()");
        return PARSE_FAIL;
    }

    if (!postfix_expression_sequence()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression()");
        return PARSE_FAIL;
    }

    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int postfix_expression_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "postfix_expression_sequence()");

    if (postfix_expression_suffix()) {
        if (!postfix_expression_sequence()) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_sequence()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_sequence()");
        return PARSE_SUCCESS;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int postfix_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "postfix_expression_suffix()");

    CC71_BacktrackingStart();

    if (CC71_GlobalTokenNumber == TokenOpenBracket) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!expression()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenCloseBracket) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_BacktrackingEnd();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
            return PARSE_SUCCESS;
        }

        if (!argument_expression_list()) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenDot) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (CC71_GlobalTokenNumber != TokenIdentifier) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenArrow) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (CC71_GlobalTokenNumber != TokenIdentifier) {
            CC71_BacktrackingRestore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
            return PARSE_FAIL;
        }

        CC71_GetToken();
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenIncrement) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenDecrement) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        CC71_BacktrackingEnd();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
        return PARSE_SUCCESS;
    }

    CC71_BacktrackingRestore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int primary_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "primary_expression()");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));

    switch(CC71_GlobalTokenNumber) {
        case TokenIdentifier:
        case TokenIntConst:
        case TokenFloatConst:
        case TokenString:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "primary_expression()");
            return PARSE_SUCCESS;

        case TokenOpenParentheses:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();

            if (!expression()) {
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "primary_expression()");
                return PARSE_FAIL;
            }

            if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "primary_expression()");
                return PARSE_FAIL;
            }

            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "primary_expression()");
            return PARSE_SUCCESS;

        default:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "primary_expression()");
            return PARSE_FAIL;
    }
}


/////////////////////////////////////////////////////////////////////////////


// OK
int type_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "type_specifier()");

    if (primitive_type_specifier()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "type_specifier()");
        return PARSE_SUCCESS;
    }

    /*if (composite_type_specifier()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "type_specifier()");
        return PARSE_SUCCESS;
    }*/
    
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "type_specifier()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int primitive_type_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "primitive_type_specifier()");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));
    
    switch (CC71_GlobalTokenNumber) {
        case TokenVoid:
        case TokenChar:
        case TokenShort:
        case TokenInt:
        case TokenLong:
        case TokenFloat:
        case TokenDouble:
        case TokenBool:
        case TokenComplex:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "primitive_type_specifier()");
            return PARSE_SUCCESS;
        default:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "primitive_type_specifier()");
            return PARSE_FAIL;
    }
}


/////////////////////////////////////////////////////////////////////////////


int composite_type_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "composite_type_specifier()");

    if (struct_or_union_specifier()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "composite_type_specifier()");
        return PARSE_SUCCESS;
    }

    if (enumerator_specifier()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "composite_type_specifier()");
        return PARSE_SUCCESS;
    }

    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int storage_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "storage_specifier()");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));
    
    switch (CC71_GlobalTokenNumber) {
        case TokenAuto:
        case TokenExtern:
        case TokenRegister:
        case TokenStatic:
        case TokenTypedef:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "storage_specifier()");
            return PARSE_SUCCESS;
        default:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "storage_specifier()");
            return PARSE_FAIL;
    }
}


/////////////////////////////////////////////////////////////////////////////


// OK
int type_qualifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "type_qualifier()");
    
    switch (CC71_GlobalTokenNumber) {
        case TokenConst:
        case TokenVolatile:
        case TokenRestrict:
        case TokenAtomic:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "type_qualifier()");
            return PARSE_SUCCESS;
        default:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "type_qualifier()");
            return PARSE_FAIL;
    }
}


/////////////////////////////////////////////////////////////////////////////


int type_qualifier_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "type_qualifier_list()");

    BACKTRACK_START();
    if (!type_qualifier()) BACKTRACK_FAIL();
    if (!type_qualifier_list_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int type_qualifier_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "type_qualifier_list_sequence()");

    BACKTRACK_START();
    if (type_qualifier()) {
        if (!type_qualifier_list_sequence()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int specifier_qualifier_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "specifier_qualifier_list()");
    
    BACKTRACK_START();
    if (!specifier_qualifier()) BACKTRACK_FAIL();
    if (!specifier_qualifier_list_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int specifier_qualifier_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "specifier_qualifier_list_sequence()");

    BACKTRACK_START();
    if (specifier_qualifier()) {
        if (!specifier_qualifier_list_sequence()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int specifier_qualifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "specifier_qualifier()");

    BACKTRACK_START();
    if (type_specifier()) {
        BACKTRACK_END();
        return 1;
    }
    if (type_qualifier()) {
        BACKTRACK_END();
        return 1;
    }
    BACKTRACK_FAIL();
}


/////////////////////////////////////////////////////////////////////////////


// OK
int type_name() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "type_name()");

    CC71_BacktrackingStart();
    
    if (!specifier_qualifier_list()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "type_name()");
        return PARSE_FAIL;
    }

    if (!type_name_suffix()) {
        CC71_BacktrackingRestore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "type_name()");
        return PARSE_FAIL;
    }

    CC71_BacktrackingEnd();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "type_name()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int type_name_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "type_name_suffix()");
    
    if (abstract_declarator()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "type_name_suffix()");
        return PARSE_SUCCESS;
    }
    
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "type_name_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int unary_operator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "unary_operator()");

    switch (CC71_GlobalTokenNumber) {
        case TokenBitwiseAnd_AddressOf:
        case TokenAsterisk:
        case TokenPlus:
        case TokenMinus:
        case TokenBitwiseNot:
        case TokenLogicalNot:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "unary_operator()");
            return PARSE_SUCCESS;
        default:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_operator()");
            return PARSE_FAIL;
    }
}


/////////////////////////////////////////////////////////////////////////////


// OK
int assignment_operator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "assignment_operator()");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));

    switch (CC71_GlobalTokenNumber) {
        case TokenAssign:
        case TokenMulAssign:
        case TokenDivAssign:
        case TokenModAssign:
        case TokenPlusAssign:
        case TokenMinusAssign:
        case TokenLeftShiftAssign:
        case TokenRightShiftAssign:
        case TokenAndAssign:
        case TokenXorAssign:
        case TokenOrAssign:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "assignment_operator()");
            return PARSE_SUCCESS;
        default:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "assignment_operator()");
            return PARSE_FAIL;
    }
}
