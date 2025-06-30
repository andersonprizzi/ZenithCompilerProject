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

#define BACKTRACK_START() marcaPosToken()
#define BACKTRACKING_RESTORE() restauraPosToken()
#define BACKTRACK_FAIL() restauraPosToken(); return 0
#define BACKTRACK_END()   /* Success */
#define PARSE_FAIL 0
#define PARSE_SUCCESS 1



/////////////////////////////////////////////////////////////////////////////
// FUNCTION IMPLEMENTATIONS                                                //
/////////////////////////////////////////////////////////////////////////////


int match(int expected) {
    if (CC71_GlobalTokenNumber == expected) {
        CC71_GetToken();
        return 1;
    }
    return 0;
}


/////////////////////////////////////////////////////////////////////////////


int translation_unit() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the translation unit() function.");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));
    
    if (!external_declaration()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "translation_unit()");
        return 0;
    }

    return translation_unit_sequence();
}


/////////////////////////////////////////////////////////////////////////////


int translation_unit_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the translation_unit_sequence() function.");
    
    BACKTRACK_START();
    if (external_declaration()) {
        if (translation_unit_sequence()) {
            return 1;
        }
    }
    BACKTRACK_FAIL();

    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int external_declaration() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the external_declaration() function.");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));

    BACKTRACK_START();
    if (function_definition()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "external_declaration()");
        return PARSE_SUCCESS;
    }
    BACKTRACK_FAIL();

    BACKTRACK_START();
    if (external_definition()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "external_declaration()");
        return PARSE_SUCCESS;
    }
    BACKTRACK_FAIL();

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "external_declaration()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


int function_definition() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the function_definition() function.");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));
    
    if (declaration_specifiers() == PARSE_FAIL) {
        return PARSE_FAIL;
    }

    if (declarator() == PARSE_FAIL) {
        return PARSE_FAIL;
    }

    if (!compound_statement()) {
        return CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, "Expected compound statement in function definition.");
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Exiting the function_definition() function.");
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int external_definition() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the external_declaration() function.");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));
    if (!declaration_specifiers()) return 0;
    return declaration_suffix();
}


/////////////////////////////////////////////////////////////////////////////


int declaration_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the declaration_specifier() function.");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));

    BACKTRACK_START();
    if (storage_specifier()) { BACKTRACK_END(); return PARSE_SUCCESS; }
    BACKTRACKING_RESTORE();

    BACKTRACK_START();
    if (type_specifier()) { BACKTRACK_END(); return PARSE_SUCCESS; }
    BACKTRACKING_RESTORE();

    BACKTRACK_START();
    if (type_qualifier()) { BACKTRACK_END(); return PARSE_SUCCESS; }
    BACKTRACKING_RESTORE();

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Exiting the declaration_specifier() function.");
    return 0;
}


/////////////////////////////////////////////////////////////////////////////


int declaration_specifiers() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the declaration_specifiers() function.");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));
    if (!declaration_specifier()) return 0;
    return declaration_specifiers_sequence();
}


/////////////////////////////////////////////////////////////////////////////


int declaration_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the declaration_suffix() function.");

    BACKTRACK_START();
    if (match(TokenSemicolon)) { BACKTRACK_END(); return 1; }
    BACKTRACK_FAIL();

    BACKTRACK_START();
    if (declaration_list()) {
        if (match(TokenSemicolon)) {
            BACKTRACK_END();
            return 1;
        }
    }
    BACKTRACK_FAIL();
    return 0;
}


/////////////////////////////////////////////////////////////////////////////


int declaration_specifiers_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the declaration_specifiers_sequence() function.");

    BACKTRACK_START();
    if (declaration_specifier()) {
        if (declaration_specifiers_sequence()) {
            return 1;
        }
    }
    BACKTRACKING_RESTORE(); // No more declaration specifiers found.

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Exiting the declaration_specifiers_sequence() function.");
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int declaration_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the declaration_list() function.");
    if (!init_declarator()) return 0;
    return declaration_list_sequence();
}


/////////////////////////////////////////////////////////////////////////////


int declaration_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the declaration_list_sequence() function.");
    BACKTRACK_START();
    if (match(TokenComma)) {
        if (init_declarator()) {
            if (declaration_list_sequence()) {
                BACKTRACK_END();
                return 1;
            }
        }
    }
    BACKTRACK_FAIL(); // epsilon
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int init_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the init_declarator() function.");
    if (!declarator()) return 0;
    return init_declarator_suffix();
}


/////////////////////////////////////////////////////////////////////////////


int init_declarator_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the init_declarator_suffix() function.");

    BACKTRACK_START();
    if (match(TokenAssign)) {
        if (initializer()) {
            BACKTRACK_END();
            return PARSE_SUCCESS;
        }
    }
    BACKTRACK_FAIL(); // epsilon
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the declarator() function.");

    BACKTRACK_START();
    if (pointer_declarator()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "declarator()");
        return PARSE_SUCCESS;
    }

    if (direct_declarator()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "declarator()");
        return PARSE_SUCCESS;
    }
    BACKTRACK_FAIL();

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "declarator()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


int pointer_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the pointer_declarator() function.");

    BACKTRACK_START();
    if (!pointer()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "pointer_declarator()");
        BACKTRACK_FAIL();
    }

    if (!direct_declarator()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "pointer_declarator()");
        BACKTRACK_FAIL();
    }
    BACKTRACK_END();

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "pointer_declarator()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int direct_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the direct_declarator() function.");

    BACKTRACK_START();
    if (!direct_declarator_base()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator()");
        BACKTRACKING_RESTORE();
        return PARSE_FAIL;
    }

    if (!direct_declarator_sequence()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator()");
        BACKTRACKING_RESTORE();
        return PARSE_FAIL;
    }
    
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int direct_declarator_base() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the direct_declarator_base() function.");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));
    
    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenIdentifier) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_base()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        
        if (!declarator()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Exiting the direct_declarator_base() function.");
        return 1;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Exiting the direct_declarator_base() function. Fail.");
    BACKTRACK_FAIL();
}


/////////////////////////////////////////////////////////////////////////////


int direct_declarator_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the direct_declarator_sequence() function.");

    BACKTRACK_START();
    if (direct_declarator_suffix()) {
        if (!direct_declarator_sequence()) {
            BACKTRACKING_RESTORE();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_sequence()");
            return PARSE_FAIL;
        }
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_sequence()");
        return PARSE_SUCCESS;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int direct_declarator_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the direct_declarator_suffix() function.");

    BACKTRACK_START();
    switch(CC71_GlobalTokenNumber) {
        case TokenOpenBracket:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();

            if (CC71_GlobalTokenNumber == TokenCloseBracket) {
                CC71_GetToken();
                return PARSE_SUCCESS;
            }

            if (!constant_expression()) BACKTRACK_FAIL();

            if (CC71_GlobalTokenNumber != TokenCloseBracket) {
                CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                BACKTRACK_FAIL();
            }

            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            BACKTRACK_END();
            return PARSE_SUCCESS;
        
        case TokenOpenParentheses:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            if (parameter_type_list()) {
                if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                    CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                    BACKTRACK_FAIL();
                }

                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                CC71_GetToken();
                BACKTRACK_END();
                return PARSE_SUCCESS;

            } else if (identifier_list()) {
                if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                    CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                    BACKTRACK_FAIL();
                }
                CC71_GetToken();
                BACKTRACK_END();
                return 1;
            } else if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                CC71_GetToken();
                BACKTRACK_END();
                return 1;
            } else {
                BACKTRACK_FAIL();
            }
        default:
            BACKTRACK_FAIL();
    }
}


/////////////////////////////////////////////////////////////////////////////


int abstract_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the abstract_declarator() function.");

    BACKTRACK_START();
    if (pointer()) {
        if (!abstract_declarator_sequence()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    if (direct_abstract_declarator()) {
        BACKTRACK_END();
        return 1;
    }
    BACKTRACK_FAIL();
}


/////////////////////////////////////////////////////////////////////////////


int abstract_declarator_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the abstract_declarator_sequence() function.");

    BACKTRACK_START();
    if (direct_abstract_declarator()) {
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int direct_abstract_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the direct_abstract_declarator() function.");

    BACKTRACK_START();
    if (!direct_abstract_declarator_base()) BACKTRACK_FAIL();
    if (!direct_abstract_declarator_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}



int direct_abstract_declarator_base() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the direct_abstract_declarator_base() function.");

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
        CC71_GetToken();
        if (CC71_GlobalTokenNumber == TokenCloseBracket) {
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




int direct_abstract_declarator_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the direct_abstract_declarator_suffix() function.");

    BACKTRACK_START();
    switch(CC71_GlobalTokenNumber) {
        case TokenOpenBracket:
            CC71_GetToken();
            if (CC71_GlobalTokenNumber == TokenCloseBracket) {
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




int pointer() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the pointer() function.");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber != TokenAsterisk) BACKTRACK_FAIL();
    CC71_GetToken();
    if (pointer_suffix()) {
        BACKTRACK_END();
        return 1;
    }
    BACKTRACK_FAIL();
}

// Pointer suffix
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
    // epsilon
    BACKTRACK_END();
    return 1;
}




int parameter_type_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the parameter_type_list() function.");
    
    BACKTRACK_START();
    if (!parameter_list()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_type_list()");
        BACKTRACK_FAIL();
    }

    if (!parameter_type_sequence()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_type_list()");
        BACKTRACK_FAIL();
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_type_list()");
    return PARSE_SUCCESS;
}




int parameter_type_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the parameter_type_sequence() function.");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_GetToken();
        if (CC71_GlobalTokenNumber == TokenEllipsis) {
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        } else {
            CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}




int parameter_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the parameter_list() function.");

    BACKTRACK_START();
    if (!parameter_declaration()) {
        BACKTRACKING_RESTORE();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_list()");
        return PARSE_FAIL;
    }

    if (!parameter_sequence()) {
        BACKTRACKING_RESTORE();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_list()");
        return PARSE_FAIL;
    }
    
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_list()");
    return PARSE_SUCCESS;
}



int parameter_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the parameter_sequence() function.");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_GetToken();
        if (!parameter_declaration()) BACKTRACK_FAIL();
        if (!parameter_sequence()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}




int parameter_declaration() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the parameter_declaration() function.");

    BACKTRACK_START();
    if (!declaration_specifiers()) {
        BACKTRACKING_RESTORE();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_declaration()");
        return PARSE_FAIL;
    }

    if (!parameter_declaration_suffix()) {
        BACKTRACKING_RESTORE();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_declaration()");
        return PARSE_FAIL;
    }
    
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_declaration()");
    return PARSE_SUCCESS;
}




int parameter_declaration_suffix() {
    BACKTRACK_START();
    if (declarator()) {
        BACKTRACK_END();
        return 1;
    }
    if (abstract_declarator()) {
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}




int identifier_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the identifier_list() function.");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber != TokenIdentifier) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "identifier_list()");
        CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
        BACKTRACK_FAIL();
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    CC71_GetToken();
    
    if (!identifier_list_sequence()) {
        BACKTRACKING_RESTORE();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "identifier_list()");
        return PARSE_FAIL;
    }
    
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "identifier_list()");
    return PARSE_SUCCESS;
}






int identifier_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the identifier_list_sequence() function.");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenIdentifier) {
            CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!identifier_list_sequence()) {
            BACKTRACK_FAIL();
        }

        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}




int initializer() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the initializer() function.");

    BACKTRACK_START();
    if (assignment_expression()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "initializer()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenOpenBrace) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!initializer_list()) {
            BACKTRACKING_RESTORE();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer()");
            return PARSE_FAIL;
        }

        if (!initializer_suffix()) {
            BACKTRACKING_RESTORE();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer()");
            return PARSE_FAIL;
        }

        return PARSE_SUCCESS;
    }

    BACKTRACKING_RESTORE();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer()");
    return PARSE_FAIL;
}






int initializer_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the initializer_suffix() function.");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenCloseBrace) {
            CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenCloseBrace) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }
    BACKTRACK_FAIL();
}






int initializer_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the initializer_list() function.");

    BACKTRACK_START();
    if (!initializer()) BACKTRACK_FAIL();
    if (!initializer_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}





int initializer_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the initializer_sequence() function.");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (!initializer()) BACKTRACK_FAIL();
        if (!initializer_sequence()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}





int struct_or_union_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the struct_or_union_specifier() function.");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenStruct || CC71_GlobalTokenNumber == TokenUnion) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();

        if (!struct_or_union_body()) {
            BACKTRACKING_RESTORE();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_or_union_specifier()");
            return PARSE_FAIL;
        }
        
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "struct_or_union_specifier()");
        return PARSE_SUCCESS;
    }

    BACKTRACKING_RESTORE();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_or_union_specifier()");
    return PARSE_FAIL;
}






int struct_or_union_body() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the struct_or_union_body() function.");

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





int field_declaration_list() {
    BACKTRACK_START();
    if (!struct_declaration()) BACKTRACK_FAIL();
    if (!field_declaration_list_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// field_declaration_list_sequence
int field_declaration_list_sequence() {
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

// struct_declaration
int struct_declaration() {
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


int struct_declarator_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered struct_declarator_list()");

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

int struct_declarator_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered struct_declarator_list_sequence()");

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


int struct_declarator_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered struct_declarator_suffix()");

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

int struct_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered struct_declarator()");

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





int enumerator_specifier() {
    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenEnum) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (!enumerator_body()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }

    BACKTRACK_FAIL();
}



int enumerator_body() {
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

// enumerator_list
int enumerator_list() {
    BACKTRACK_START();
    if (!enumerator()) BACKTRACK_FAIL();
    if (!enumerator_list_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// enumerator_list_sequence
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

// enumerator
int enumerator() {
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

// enumerator_suffix
int enumerator_suffix() {
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



int compound_statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "compound_statement()");

    BACKTRACK_START();
    if (CC71_GlobalTokenNumber != TokenOpenBrace) BACKTRACK_FAIL();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    CC71_GetToken();

    // Opções:
    // 1) TokenCloseBrace
    if (CC71_GlobalTokenNumber == TokenCloseBrace) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }

    // 2) declaration_list statement_list TokenCloseBrace
    if (declaration_list()) {
        if (statement_list()) {
            if (CC71_GlobalTokenNumber == TokenCloseBrace) {
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                CC71_GetToken();
                BACKTRACK_END();
                return 1;
            }
            CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        // Se não for statement_list, tenta só declaration_list + TokenCloseBrace
        if (CC71_GlobalTokenNumber == TokenCloseBrace) {
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        }
        CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
        BACKTRACK_FAIL();
    }

    // 3) statement_list TokenCloseBrace
    if (statement_list()) {
        if (CC71_GlobalTokenNumber == TokenCloseBrace) {
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        }
        CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
        BACKTRACK_FAIL();
    }

    CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
    BACKTRACK_FAIL();
}





int statement_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the statement_list() function.");
    BACKTRACK_START();
    if (!statement()) BACKTRACK_FAIL();
    if (!statement_list_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}




int statement_list_sequence() {
    BACKTRACK_START();
    if (statement()) {
        if (!statement_list_sequence()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}

// statement
int statement() {
    BACKTRACK_START();
    if (labeled_statement()) {
        BACKTRACK_END();
        return 1;
    }
    if (compound_statement()) {
        BACKTRACK_END();
        return 1;
    }
    if (expression_statement()) {
        BACKTRACK_END();
        return 1;
    }
    if (selection_statement()) {
        BACKTRACK_END();
        return 1;
    }
    if (iteration_statement()) {
        BACKTRACK_END();
        return 1;
    }
    if (jump_statement()) {
        BACKTRACK_END();
        return 1;
    }
    BACKTRACK_FAIL();
}




int labeled_statement() {
    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenIdentifier) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
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


// expression_statement
int expression_statement() {
    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenSemicolon) {
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }
    if (expression()) {
        if (CC71_GlobalTokenNumber == TokenSemicolon) {
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        }
        CC71_ReportError(CC71_ERR_SYN_MISSING_SEMICOLON, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
        BACKTRACK_FAIL();
    }
    BACKTRACK_FAIL();
}

// selection_statement
int selection_statement() {
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

// selection_statement_else_opt
int selection_statement_else_opt() {
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

// iteration_statement
int iteration_statement() {
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

// jump_statement
int jump_statement() {
    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenGoto) {
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenIdentifier) {
            CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
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
    if (CC71_GlobalTokenNumber == TokenContinue) {
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenSemicolon) {
            CC71_ReportError(CC71_ERR_SYN_MISSING_SEMICOLON, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
            BACKTRACK_FAIL();
        }
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenBreak) {
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
        CC71_GetToken();
        if (CC71_GlobalTokenNumber == TokenSemicolon) {
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        }
        if (expression()) {
            if (CC71_GlobalTokenNumber != TokenSemicolon) {
                CC71_ReportError(CC71_ERR_SYN_MISSING_SEMICOLON, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
                BACKTRACK_FAIL();
            }
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        }
        CC71_ReportError(CC71_ERR_SYN_MISSING_SEMICOLON, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, NULL);
        BACKTRACK_FAIL();
    }
    BACKTRACK_FAIL();
}


int expression() {
    if (!assignment_expression()) {
        CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn,
                         "Expected assignment_expression in expression.");
        return 0;
    }
    if (!expression_sequence()) {
        // expression_sequence pode ser vazio, então falha aqui só se houver erro real
        // Geralmente, expressão_sequence retorna 1 mesmo no caso epsilon
        return 0;
    }
    return 1;
}

int expression_sequence() {
    BACKTRACK_START();

    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_GetToken();  // consome a vírgula

        if (!assignment_expression()) {
            CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn,
                             "Expected assignment_expression after ','.");
            BACKTRACK_FAIL();
        }

        if (!expression_sequence()) {
            BACKTRACK_FAIL();
        }

        BACKTRACK_END();
        return 1;
    }

    // ε (vazio)
    BACKTRACK_END();
    return 1;
}


// constant_expression : conditional_expression
int constant_expression() {
    BACKTRACK_START();
    if (!conditional_expression()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// argument_expression_list : assignment_expression argument_expression_sequence
int argument_expression_list() {
    BACKTRACK_START();
    if (!assignment_expression()) BACKTRACK_FAIL();
    if (!argument_expression_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// argument_expression_sequence : ',' assignment_expression argument_expression_sequence | ε
int argument_expression_sequence() {
    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_GetToken();
        if (!assignment_expression()) BACKTRACK_FAIL();
        if (!argument_expression_sequence()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}

// assignment_expression
int assignment_expression() {
    BACKTRACK_START();

    // tentativa 1: unary_expression assignment_operator assignment_expression
    BACKTRACK_START();
    if (unary_expression()) {
        if (assignment_operator()) {
            if (assignment_expression()) {
                BACKTRACK_END();
                return 1;
            }
        }
    }
    BACKTRACK_FAIL();

    // tentativa 2: conditional_expression
    if (conditional_expression()) {
        BACKTRACK_END();
        return 1;
    }

    BACKTRACK_FAIL();
}

// conditional_expression : logical_or_expression conditional_expression_suffix
int conditional_expression() {
    BACKTRACK_START();
    if (!logical_or_expression()) BACKTRACK_FAIL();
    if (!conditional_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// conditional_expression_suffix : '?' expression ':' conditional_expression | ε
int conditional_expression_suffix() {
    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenQuestionMark) {
        CC71_GetToken();
        if (!expression()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenColon) BACKTRACK_FAIL();
        CC71_GetToken();
        if (!conditional_expression()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}

// logical_or_expression : logical_and_expression logical_or_expression_suffix
int logical_or_expression() {
    BACKTRACK_START();
    if (!logical_and_expression()) BACKTRACK_FAIL();
    if (!logical_or_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// logical_or_expression_suffix : '||' logical_and_expression logical_or_expression_suffix | ε
int logical_or_expression_suffix() {
    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenLogicalOr) {
        CC71_GetToken();
        if (!logical_and_expression()) BACKTRACK_FAIL();
        if (!logical_or_expression_suffix()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}

// logical_and_expression : inclusive_or_expression logical_and_expression_suffix
int logical_and_expression() {
    BACKTRACK_START();
    if (!inclusive_or_expression()) BACKTRACK_FAIL();
    if (!logical_and_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// logical_and_expression_suffix : '&&' inclusive_or_expression logical_and_expression_suffix | ε
int logical_and_expression_suffix() {
    BACKTRACK_START();
    if (CC71_GlobalTokenNumber == TokenLogicalAnd) {
        CC71_GetToken();
        if (!inclusive_or_expression()) BACKTRACK_FAIL();
        if (!logical_and_expression_suffix()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}

// inclusive_or_expression : exclusive_or_expression inclusive_or_expression_suffix
int inclusive_or_expression() {
    BACKTRACK_START();
    if (!exclusive_or_expression()) BACKTRACK_FAIL();
    if (!inclusive_or_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// inclusive_or_expression_suffix : '|' exclusive_or_expression inclusive_or_expression_suffix | ε
int inclusive_or_expression_suffix() {
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

// exclusive_or_expression : and_expression exclusive_or_expression_suffix
int exclusive_or_expression() {
    BACKTRACK_START();
    if (!and_expression()) BACKTRACK_FAIL();
    if (!exclusive_or_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// exclusive_or_expression_suffix : '^' and_expression exclusive_or_expression_suffix | ε
int exclusive_or_expression_suffix() {
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

// and_expression : equality_expression and_expression_suffix
int and_expression() {
    BACKTRACK_START();
    if (!equality_expression()) BACKTRACK_FAIL();
    if (!and_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// and_expression_suffix : '&' equality_expression and_expression_suffix | ε
int and_expression_suffix() {
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

// equality_expression : relational_expression equality_expression_suffix
int equality_expression() {
    BACKTRACK_START();
    if (!relational_expression()) BACKTRACK_FAIL();
    if (!equality_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// equality_expression_suffix : '==' relational_expression equality_expression_suffix | '!=' relational_expression equality_expression_suffix | ε
int equality_expression_suffix() {
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

// relational_expression : shift_expression relational_expression_suffix
int relational_expression() {
    BACKTRACK_START();
    if (!shift_expression()) BACKTRACK_FAIL();
    if (!relational_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// relational_expression_suffix : '<' shift_expression relational_expression_suffix | '>' shift_expression relational_expression_suffix | '<=' shift_expression relational_expression_suffix | '>=' shift_expression relational_expression_suffix | ε
int relational_expression_suffix() {
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

// shift_expression : additive_expression shift_expression_suffix
int shift_expression() {
    BACKTRACK_START();
    if (!additive_expression()) BACKTRACK_FAIL();
    if (!shift_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// shift_expression_suffix : '<<' additive_expression shift_expression_suffix | '>>' additive_expression shift_expression_suffix | ε
int shift_expression_suffix() {
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

// additive_expression : multiplicative_expression additive_expression_suffix
int additive_expression() {
    BACKTRACK_START();
    if (!multiplicative_expression()) BACKTRACK_FAIL();
    if (!additive_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// additive_expression_suffix : '+' multiplicative_expression additive_expression_suffix | '-' multiplicative_expression additive_expression_suffix | ε
int additive_expression_suffix() {
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

// multiplicative_expression : cast_expression multiplicative_expression_suffix
int multiplicative_expression() {
    BACKTRACK_START();
    if (!cast_expression()) BACKTRACK_FAIL();
    if (!multiplicative_expression_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// multiplicative_expression_suffix : '*' cast_expression multiplicative_expression_suffix | '/' cast_expression multiplicative_expression_suffix | '%' cast_expression multiplicative_expression_suffix | ε
int multiplicative_expression_suffix() {
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



int cast_expression() {
    BACKTRACK_START();

    BACKTRACK_START();
    if (unary_expression()) {
        BACKTRACK_END();
        return 1;
    }
    BACKTRACK_FAIL();

    if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        CC71_GetToken();
        if (!type_name()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseParentheses) BACKTRACK_FAIL();
        CC71_GetToken();
        if (!cast_expression()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }

    BACKTRACK_FAIL();
}

// unary_expression : postfix_expression
// | '++' unary_expression
// | '--' unary_expression
// | unary_operator cast_expression
// | 'sizeof' unary_expression
// | 'sizeof' '(' type_name ')'
int unary_expression() {
    BACKTRACK_START();

    if (CC71_GlobalTokenNumber == TokenIncrement) {
        CC71_GetToken();
        if (!unary_expression()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }

    if (CC71_GlobalTokenNumber == TokenDecrement) {
        CC71_GetToken();
        if (!unary_expression()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }

    if (unary_operator()) {
        if (!cast_expression()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }

    if (CC71_GlobalTokenNumber == TokenSizeof) {
        CC71_GetToken();
        if (unary_expression()) {
            BACKTRACK_END();
            return 1;
        }
        if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
            CC71_GetToken();
            if (!type_name()) BACKTRACK_FAIL();
            if (CC71_GlobalTokenNumber != TokenCloseParentheses) BACKTRACK_FAIL();
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        }
        BACKTRACK_FAIL();
    }

    if (postfix_expression()) {
        BACKTRACK_END();
        return 1;
    }

    BACKTRACK_FAIL();
}

// postfix_expression : primary_expression postfix_expression_sequence
int postfix_expression() {
    BACKTRACK_START();
    if (!primary_expression()) BACKTRACK_FAIL();
    if (!postfix_expression_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}

// postfix_expression_sequence : postfix_expression_suffix postfix_expression_sequence | ε
int postfix_expression_sequence() {
    BACKTRACK_START();
    if (postfix_expression_suffix()) {
        if (!postfix_expression_sequence()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}





int postfix_expression_suffix() {
    BACKTRACK_START();

    if (CC71_GlobalTokenNumber == TokenOpenBracket) {
        CC71_GetToken();
        if (!expression()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseBracket) BACKTRACK_FAIL();
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_GetToken();
        if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        }
        if (!argument_expression_list()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseParentheses) BACKTRACK_FAIL();
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenDot) {
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenIdentifier) BACKTRACK_FAIL();
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenArrow) {
        CC71_GetToken();
        if (CC71_GlobalTokenNumber != TokenIdentifier) BACKTRACK_FAIL();
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenIncrement) {
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenDecrement) {
        CC71_GetToken();
        BACKTRACK_END();
        return 1;
    }

    BACKTRACK_FAIL();
}

// primary_expression : TokenIdentifier | TokenIntConst | TokenFloatConst | TokenString | '(' expression ')'
int primary_expression() {
    BACKTRACK_START();
    switch(CC71_GlobalTokenNumber) {
        case TokenIdentifier:
        case TokenIntConst:
        case TokenFloatConst:
        case TokenString:
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        case TokenOpenParentheses:
            CC71_GetToken();
            if (!expression()) BACKTRACK_FAIL();
            if (CC71_GlobalTokenNumber != TokenCloseParentheses) BACKTRACK_FAIL();
            CC71_GetToken();
            BACKTRACK_END();
            return 1;
        default:
            BACKTRACK_FAIL();
    }
}



int type_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the type_specifier() function.");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));

    BACKTRACK_START();
    if (primitive_type_specifier()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Exiting the type_specifier() function.");
        return 1;
    }

    if (composite_type_specifier()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the type_specifier() function.");
        return 1;
    }
    BACKTRACK_FAIL();
}


int primitive_type_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the primitive_type_specifier() function.");
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
            return 1;
        default:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Exiting the primitive_type_specifier() function. No valid primitive type specifier found.");
    }
}



int composite_type_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the composite_type_specifier() function.");

    BACKTRACK_START();
    if (struct_or_union_specifier()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Exiting the composite_type_specifier() function.");
        return 1;
    }
    if (enumerator_specifier()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Exiting the composite_type_specifier() function.");
        return 1;
    }
    BACKTRACK_FAIL();
}




int storage_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the storage_specifier() function.");
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Current token: %s", CC71_TokenToString(CC71_GlobalTokenNumber));
    
    switch (CC71_GlobalTokenNumber) {
        case TokenAuto:
        case TokenExtern:
        case TokenRegister:
        case TokenStatic:
        case TokenTypedef:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            return 1;
        default:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Exiting the storage_specifier() function. No valid storage specifier found.");
            return 0;
    }
}



int type_qualifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the type_qualifier() function.");

    BACKTRACK_START();
    switch (CC71_GlobalTokenNumber) {
        case TokenConst:
        case TokenVolatile:
        case TokenRestrict:
        case TokenAtomic:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            return 1;
        default:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Exiting the type_qualifier() function. No valid type qualifier found.");
            return 0;
    }
}


/////////////////////////////////////////////////////////////////////////////


int type_qualifier_list() {
    BACKTRACK_START();
    if (!type_qualifier()) BACKTRACK_FAIL();
    if (!type_qualifier_list_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int type_qualifier_list_sequence() {
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
    BACKTRACK_START();
    if (!specifier_qualifier()) BACKTRACK_FAIL();
    if (!specifier_qualifier_list_sequence()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int specifier_qualifier_list_sequence() {
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


int type_name() {
    BACKTRACK_START();
    if (!specifier_qualifier_list()) BACKTRACK_FAIL();
    if (!type_name_suffix()) BACKTRACK_FAIL();
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int type_name_suffix() {
    BACKTRACK_START();
    if (abstract_declarator()) {
        BACKTRACK_END();
        return 1;
    }
    // epsilon
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int unary_operator() {
    switch (CC71_GlobalTokenNumber) {
        case TokenBitwiseAnd_AddressOf:
        case TokenAsterisk:
        case TokenPlus:
        case TokenMinus:
        case TokenBitwiseNot:
        case TokenLogicalNot:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            CC71_GetToken();
            return 1;
        default:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Exiting the unary_operator() function. No valid unary operator found.");
    }
}


/////////////////////////////////////////////////////////////////////////////


int assignment_operator() {
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
            return 1;
        default:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Exiting the assignment_operator() function. No valid assignment operator found.");
    }
}
