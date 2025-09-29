/////////////////////////////////////////////////////////////////////////////
// LIBRARY INCLUDES                                                        //
/////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "logger.h"
#include "error.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "lowerer.h"

/////////////////////////////////////////////////////////////////////////////
// PREPROCESSING DEFINITIONS                                               //
/////////////////////////////////////////////////////////////////////////////

#define BACKTRACKING_RESTORE() zenith_backtracking_restore()
#define BACKTRACK_FAIL() zenith_backtracking_restore(); return 0
#define BACKTRACK_END()   zenith_backtracking_end()
#define PARSE_FAIL 0
#define PARSE_SUCCESS 1
#define FORCE_ERROR_THROW 1



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
    
    zenith_backtracking_start();

    CC71_SilentMode = 0;

    if (function_definition()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "external_declaration()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    zenith_backtracking_start();

    CC71_SilentMode = 0;

    if (external_definition()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "external_declaration()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
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
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "function_definition()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int external_definition() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "external_definition()");

    zenith_backtracking_start();

    if (!declaration_specifiers()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "external_definition()");
        return PARSE_FAIL;
    }

    declaration_suffix();

    if (CC71_GlobalTokenNumber != TokenSemicolon) {
        CC71_ReportExpectedTokenError(TokenSemicolon, FORCE_ERROR_THROW);
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "external_definition()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    zenith_get_token();

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "external_definition()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int declaration_specifiers() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "declaration_specifiers()");
    
    if (!declaration_specifier()) {
        CC71_ReportError(
            CC71_ERR_SYN_UNEXPECTED_TOKEN,
            CC71_GlobalCurrentLine,
            columnAux,
            "Expected declaration specifier but found '%s'.",
            CC71_TokenToString(CC71_GlobalTokenNumber)
        );

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

    zenith_backtracking_start();

    if (!init_declarator()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "declaration_suffix() [ε]");
        return PARSE_SUCCESS;
    }

    while (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!init_declarator()) {
            CC71_SilentMode = 0;
            CC71_ReportError(
                CC71_ERR_SYN_UNEXPECTED_TOKEN,
                CC71_GlobalCurrentLine,
                columnAux,
                "Expected TokenAsterisk, TokenIdentifier or TokenOpenParentheses but found '%s'.",
                CC71_TokenToString(CC71_GlobalTokenNumber)
            );
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "declaration_suffix()");
            return PARSE_FAIL;
        }
    }
    
    zenith_backtracking_end();
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

    zenith_backtracking_start();

    if (external_definition()) {
        if (!declaration_list_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "declaration_list_sequence()");
            return PARSE_FAIL;
        }
        
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "declaration_list_sequence()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_restore();
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

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenAssign) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!initializer()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "init_declarator_sequence()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "init_declarator_sequence()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_end();
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

    zenith_backtracking_start();

    if (!pointer()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "pointer_declarator()");
        zenith_backtracking_restore();
        return PARSE_FAIL;
    }

    if (!direct_declarator()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "pointer_declarator()");
        zenith_backtracking_restore();
        return PARSE_FAIL;
    }

    zenith_backtracking_end();

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "pointer_declarator()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int direct_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "direct_declarator()");

    zenith_backtracking_start();

    if (!direct_declarator_base()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator()");
        return PARSE_FAIL;
    }

    if (!direct_declarator_sequence()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator()");

        return PARSE_FAIL;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int direct_declarator_base() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "direct_declarator_base()");
    
    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenIdentifier) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_base()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        
        if (!declarator()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_base()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportExpectedTokenError(TokenCloseParentheses);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_base()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_base()");
        zenith_backtracking_end();
        return PARSE_SUCCESS;
    }

    /*CC71_ReportError(
        CC71_ERR_SYN_UNEXPECTED_TOKEN,
        CC71_GlobalCurrentLine,
        columnAux,
        "Expected TokenIdentifier or TokenOpenParentheses but found '%s'.",
        CC71_TokenToString(CC71_GlobalTokenNumber)
    );*/

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_base()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int direct_declarator_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "direct_declarator_sequence()");

    zenith_backtracking_start();

    CC71_SilentMode++;

    if (direct_declarator_suffix()) {
        if (!direct_declarator_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_sequence()");
            return PARSE_FAIL;
        }

        CC71_SilentMode--;
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_sequence()");
        return PARSE_SUCCESS;
    }

    CC71_SilentMode--;
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int direct_declarator_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "direct_declarator_suffix()");

    zenith_backtracking_start();

    switch (CC71_GlobalTokenNumber) {
        case TokenOpenBracket:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();

            if (CC71_GlobalTokenNumber == TokenCloseBracket) {
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                zenith_get_token();
                zenith_backtracking_end();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_suffix()");
                return PARSE_SUCCESS;
            }

            if (!constant_expression()) {
                zenith_backtracking_restore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_suffix()");
                return PARSE_FAIL;
            }

            if (CC71_GlobalTokenNumber != TokenCloseBracket) {
                CC71_ReportExpectedTokenError(TokenCloseBracket);
                zenith_backtracking_restore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_suffix()");
                return PARSE_FAIL;
            }

            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();
            zenith_backtracking_end();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_suffix()");
            return PARSE_SUCCESS;

        case TokenOpenParentheses:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();

            if (parameter_type_list()) {
                if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                    CC71_ReportExpectedTokenError(TokenCloseParentheses);
                    zenith_backtracking_restore();
                    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_suffix()");
                    return PARSE_FAIL;
                }

                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                zenith_get_token();
                zenith_backtracking_end();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_suffix()");
                return PARSE_SUCCESS;

            } else if (identifier_list()) {
                if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                    ////CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, columnAux, NULL);
                    zenith_backtracking_restore();
                    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_suffix()");
                    return PARSE_FAIL;
                }

                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                zenith_get_token();
                zenith_backtracking_end();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_suffix()");
                return PARSE_SUCCESS;

            } else if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                zenith_get_token();
                zenith_backtracking_end();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_declarator_suffix()");
                return PARSE_SUCCESS;
            }

            //CC71_ReportExpectedTokenError(TokenCloseParentheses);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_suffix()");
            return PARSE_FAIL;

        default:
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "direct_declarator_suffix()");
            return PARSE_FAIL;
    }
}


/////////////////////////////////////////////////////////////////////////////


// OK
int abstract_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "abstract_declarator()");

    zenith_backtracking_start();

    if (pointer()) {
        if (!abstract_declarator_sequence()) BACKTRACK_FAIL();
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "abstract_declarator()");
        return PARSE_SUCCESS;
    }

    if (direct_abstract_declarator()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "abstract_declarator()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "abstract_declarator()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int abstract_declarator_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "abstract_declarator_sequence()");

    zenith_backtracking_start();

    if (direct_abstract_declarator()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "abstract_declarator_sequence()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "abstract_declarator_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int direct_abstract_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "direct_abstract_declarator()");

    zenith_backtracking_start();

    if (!direct_abstract_declarator_base()) BACKTRACK_FAIL();
    if (!direct_abstract_declarator_sequence()) BACKTRACK_FAIL();

    zenith_backtracking_end();

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "direct_abstract_declarator()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int direct_abstract_declarator_base() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "direct_abstract_declarator_base()");

    zenith_backtracking_start();
    if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        if (!abstract_declarator()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            ////CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, columnAux, NULL);
            BACKTRACK_FAIL();
        }
        zenith_get_token();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenOpenBracket) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber == TokenCloseBracket) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();
            BACKTRACK_END();
            return 1;
        }
        if (!constant_expression()) BACKTRACK_FAIL();
        if (CC71_GlobalTokenNumber != TokenCloseBracket) {
            ////CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, columnAux, NULL);
            BACKTRACK_FAIL();
        }
        zenith_get_token();
        BACKTRACK_END();
        return 1;
    }
    if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        if (parameter_type_list()) {
            if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                ////CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, columnAux, NULL);
                BACKTRACK_FAIL();
            }
            zenith_get_token();
            BACKTRACK_END();
            return 1;
        } else if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
            zenith_get_token();
            BACKTRACK_END();
            return 1;
        }
        BACKTRACK_FAIL();
    }
    BACKTRACK_FAIL();
}


/////////////////////////////////////////////////////////////////////////////


int direct_abstract_declarator_sequence() {
    //CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the direct_abstract_declarator_sequence() function.");

    zenith_backtracking_start();
    if (direct_abstract_declarator_suffix()) {
        if (!direct_abstract_declarator_sequence()) BACKTRACK_FAIL();
        BACKTRACK_END();
        return 1;
    }
    
    BACKTRACK_END();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////


int direct_abstract_declarator_suffix() {
    //CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "Entered the direct_abstract_declarator_suffix() function.");

    zenith_backtracking_start();
    switch(CC71_GlobalTokenNumber) {
        case TokenOpenBracket:
            zenith_get_token();
            if (CC71_GlobalTokenNumber == TokenCloseBracket) {
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                zenith_get_token();
                BACKTRACK_END();
                return 1;
            }
            if (!constant_expression()) BACKTRACK_FAIL();
            if (CC71_GlobalTokenNumber != TokenCloseBracket) {
                ////CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, columnAux, NULL);
                BACKTRACK_FAIL();
            }
            zenith_get_token();
            BACKTRACK_END();
            return 1;
        case TokenOpenParentheses:
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();
            if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
                zenith_get_token();
                BACKTRACK_END();
                return 1;
            }
            if (parameter_type_list()) {
                if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                    ////CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, columnAux, NULL);
                    BACKTRACK_FAIL();
                }
                zenith_get_token();
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

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber != TokenAsterisk) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "pointer()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    zenith_get_token();

    if (pointer_suffix()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "pointer()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "pointer()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int pointer_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "pointer_suffix()");

    zenith_backtracking_start();

    if (type_qualifier_list()) {
        if (pointer()) {
            zenith_backtracking_end();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "pointer_suffix()");
            return PARSE_SUCCESS;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "pointer_suffix()");
        return PARSE_SUCCESS;
    }

    if (pointer()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "pointer_suffix()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "pointer_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int parameter_type_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "parameter_type_list()");

    zenith_backtracking_start();
    if (parameter_list()) {
        if (CC71_GlobalTokenNumber == TokenComma) {
            zenith_get_token();
            if (CC71_GlobalTokenNumber == TokenEllipsis) {
                zenith_get_token();
                zenith_backtracking_end();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_type_list() with , ...");
                return PARSE_SUCCESS;
            }
        }
    } else if (CC71_GlobalTokenNumber == TokenEllipsis) {
        CC71_ReportError(
            CC71_ERR_SYN_UNEXPECTED_TOKEN,
            CC71_GlobalCurrentLine,
            columnAux,
            "Expected at least one parameter before the Ellipses Token (...)."
        );
    }

    zenith_backtracking_restore();
    zenith_backtracking_start();

    if (parameter_list()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_type_list() without , ...");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_type_list() failed");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int parameter_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "parameter_list()");

    zenith_backtracking_start();

    if (!parameter_declaration()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_list()");
        return PARSE_FAIL;
    }

    if (!parameter_sequence()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_list()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_list()");
    zenith_backtracking_end();
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int parameter_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "parameter_sequence()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenComma) {
        zenith_get_token();
        
        if (CC71_GlobalTokenNumber == TokenEllipsis) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_sequence() ends before ellipsis");
            return PARSE_SUCCESS; // ε
        }

        if (!parameter_declaration()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_sequence()");
            return PARSE_FAIL;
        }

        if (!parameter_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_sequence()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_sequence()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_sequence() is empty");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int parameter_declaration() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "parameter_declaration()");

    zenith_backtracking_start();

    if (!declaration_specifiers()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_declaration()");
        return PARSE_FAIL;
    }

    if (!parameter_declaration_suffix()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "parameter_declaration()");
        return PARSE_FAIL;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_declaration()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int parameter_declaration_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "parameter_declaration_suffix()");

    zenith_backtracking_start();

    if (declarator()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_declaration_suffix()");
        return PARSE_SUCCESS;
    }

    if (abstract_declarator()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_declaration_suffix()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "parameter_declaration_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int identifier_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "identifier_list()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber != TokenIdentifier) {
        ////CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, columnAux, NULL);
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "identifier_list()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    zenith_get_token();

    if (!identifier_list_sequence()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "identifier_list()");
        return PARSE_FAIL;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "identifier_list()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int identifier_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "identifier_list_sequence()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenIdentifier) {
            ////CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, columnAux, NULL);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "identifier_list_sequence()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!identifier_list_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "identifier_list_sequence()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "identifier_list_sequence()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "identifier_list_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int initializer() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "initializer()");

    zenith_backtracking_start();
    
    if (assignment_expression()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "initializer()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenOpenBrace) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!initializer_list()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer()");
            return PARSE_FAIL;
        }

        if (!initializer_suffix()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "initializer()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int initializer_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "initializer_suffix()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenCloseBrace) {
            ////CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, columnAux, NULL);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer_suffix()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "initializer_suffix()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenCloseBrace) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "initializer_suffix()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer_suffix()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int initializer_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "initializer_list()");

    zenith_backtracking_start();

    if (!initializer()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer_list()");
        return PARSE_FAIL;
    }

    if (!initializer_sequence()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer_list()");
        return PARSE_FAIL;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "initializer_list()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int initializer_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "initializer_sequence()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!initializer()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer_sequence()");
            return PARSE_FAIL;
        }

        if (!initializer_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "initializer_sequence()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "initializer_sequence()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "initializer_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int struct_or_union_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_or_union_specifier()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenStruct || CC71_GlobalTokenNumber == TokenUnion) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!struct_or_union_body()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_or_union_specifier()");
            return PARSE_FAIL;
        }
        
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "struct_or_union_specifier()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_or_union_specifier()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int struct_or_union_body() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_or_union_body()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenIdentifier) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber == TokenOpenBrace) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();

            if (!field_declaration_list()) {
                zenith_backtracking_restore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_or_union_body()");
                return PARSE_FAIL;
            }

            if (CC71_GlobalTokenNumber != TokenCloseBrace) {
                //(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, columnAux, NULL);
                zenith_backtracking_restore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_or_union_body()");
                return PARSE_FAIL;
            }

            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "struct_or_union_body()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenOpenBrace) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!field_declaration_list()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_or_union_body()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenCloseBrace) {
            //CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, columnAux, NULL);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_or_union_body()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "struct_or_union_body()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_or_union_body()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int field_declaration_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "field_declaration_list()");

    if (!struct_declaration()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "field_declaration_list()");
        return PARSE_FAIL;
    }

    if (!field_declaration_list_sequence()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "field_declaration_list()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "field_declaration_list()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int field_declaration_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "field_declaration_list_sequence()");

    zenith_backtracking_start();

    if (struct_declaration()) {
        if (!field_declaration_list_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "field_declaration_list_sequence()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "field_declaration_list_sequence()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "field_declaration_list_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int struct_declaration() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_declaration()");

    zenith_backtracking_start();

    if (!specifier_qualifier_list()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_declaration()");
        return PARSE_FAIL;
    }

    if (!struct_declarator_list()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_declaration()");
        return PARSE_FAIL;
    }

    if (CC71_GlobalTokenNumber != TokenSemicolon) {
        //CC71_ReportError(CC71_ERR_SYN_MISSING_SEMICOLON, CC71_GlobalCurrentLine, columnAux, NULL);
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_declaration()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    zenith_get_token();

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "struct_declaration()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int struct_declarator_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_declarator_list()");

    zenith_backtracking_start();

    if (!struct_declarator()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_declarator_list()");
        return PARSE_FAIL;
    }
    if (!struct_declarator_list_sequence()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_declarator_list()");
        return PARSE_FAIL;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "struct_declarator_list()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int struct_declarator_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_declarator_list_sequence()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!struct_declarator()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_declarator_list_sequence()");
            return PARSE_FAIL;
        }

        if (!struct_declarator_list_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_declarator_list_sequence()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "struct_declarator_list_sequence()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "struct_declarator_list_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int struct_declarator_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_declarator_suffix()");

    zenith_backtracking_start();
    if (CC71_GlobalTokenNumber == TokenColon) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!constant_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_declarator_suffix()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "struct_declarator_suffix()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "struct_declarator_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int struct_declarator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "struct_declarator()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenColon) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!constant_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_declarator()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "struct_declarator()");
        return PARSE_SUCCESS;
    
    } else {
        if (!declarator()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_declarator()");
            return PARSE_FAIL;
        }

        if (!struct_declarator_suffix()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "struct_declarator()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "struct_declarator()");
        return PARSE_SUCCESS;
    }
}


/////////////////////////////////////////////////////////////////////////////


// OK
int enumerator_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "enumerator_specifier()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenEnum) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (enumerator_body()) {
            zenith_backtracking_end();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "enumerator_specifier()");
            return PARSE_SUCCESS;
        }

        if (CC71_GlobalTokenNumber == TokenIdentifier) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();

            zenith_backtracking_end();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "enumerator_specifier()");
            return PARSE_SUCCESS;
        }
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator_specifier()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int enumerator_body() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "enumerator_body()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenOpenBrace) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!enumerator_list()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator_body()");
            return PARSE_FAIL;
        }
        
        if (CC71_GlobalTokenNumber != TokenCloseBrace) {
            //CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, columnAux, NULL);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator_body()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "enumerator_body()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenIdentifier) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber == TokenOpenBrace) {
            zenith_get_token();
            if (!enumerator_list()) {
                zenith_backtracking_restore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator_body()");
                return PARSE_FAIL;
            }

            if (CC71_GlobalTokenNumber != TokenCloseBrace) {
                //CC71_ReportError(CC71_ERR_SYN_EXPECTED_CLOSE_PAREN, CC71_GlobalCurrentLine, columnAux, NULL);
                zenith_backtracking_restore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator_body()");
                return PARSE_FAIL;
            }

            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();

            zenith_backtracking_end();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "enumerator_body()");
            return PARSE_SUCCESS;
        }

        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator_body()");
        return PARSE_FAIL;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator_body()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int enumerator_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "enumerator_list()");

    if (!enumerator()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator_list()");
        return PARSE_FAIL;
    }
    
    if (!enumerator_list_sequence()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator_list()");
        return PARSE_FAIL;
    }
    
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "enumerator_list()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int enumerator_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "enumerator_list_sequence()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!enumerator()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator_list_sequence()");
            return PARSE_FAIL;
        }

        if (!enumerator_list_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator_list_sequence()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "enumerator_list_sequence()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "enumerator_list_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int enumerator() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "enumerator()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber != TokenIdentifier) {
        //CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, columnAux, NULL);
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    zenith_get_token();

    if (!enumerator_suffix()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator()");
        return PARSE_FAIL;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "enumerator()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int enumerator_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "enumerator_suffix()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenAssign) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!constant_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "enumerator_suffix()");
            return PARSE_FAIL;
        }
        
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "enumerator_suffix()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "enumerator_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int compound_statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "compound_statement()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber != TokenOpenBrace) {
        //CC71_ReportExpectedTokenError(TokenOpenBrace);
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "compound_statement()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    zenith_get_token();

    if (!block_item_sequence()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "compound_statement()");
        return PARSE_FAIL;
    }

    if (CC71_GlobalTokenNumber != TokenCloseBrace) {
        CC71_ReportExpectedTokenError(TokenCloseBrace), FORCE_ERROR_THROW;
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "compound_statement()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
    zenith_get_token();

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "compound_statement()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int block_item_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "block_item_sequence()");

    zenith_backtracking_start();

    if (block_item()) {
        if (!block_item_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "block_item_sequence()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "block_item_sequence()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "block_item_sequence() [epsilon]");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int block_item() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "block_item()");

    zenith_backtracking_start();

    CC71_SilentMode++;

    if (external_definition()) {
        CC71_SilentMode--;
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "block_item() [external_definition]");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    zenith_backtracking_start();

    if (statement()) {
        CC71_SilentMode--;
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "block_item() [statement]");
        return PARSE_SUCCESS;
    }

    CC71_SilentMode--;
    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "block_item()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int statement_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "statement_list()");

    zenith_backtracking_start();

    if (!statement()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "statement_list()");
        return PARSE_FAIL;
    }

    if (!statement_list_sequence()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "statement_list()");
        return PARSE_FAIL;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement_list()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int statement_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "statement_list_sequence()");

    zenith_backtracking_start();
    
    if (statement()) {
        if (!statement_list_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "statement_list_sequence()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement_list_sequence()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement_list_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "statement()");

    zenith_backtracking_start();

    if (labeled_statement()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement()");
        return PARSE_SUCCESS;
    }

    else if (compound_statement()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement()");
        return PARSE_SUCCESS;
    }

    else if (expression_statement()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement()");
        return PARSE_SUCCESS;
    }

    else if (selection_statement()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement()");
        return PARSE_SUCCESS;
    }

    else if (iteration_statement()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement()");
        return PARSE_SUCCESS;
    }

    else if (jump_statement()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "statement()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "statement()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int labeled_statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "labeled_statement()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenIdentifier) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenColon) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "labeled_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!statement()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "labeled_statement()");
            return PARSE_FAIL;
        }
        
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "labeled_statement()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenCase) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!constant_expression()) {
            CC71_SilentMode = 0;

            CC71_ReportError(
                CC71_ERR_SYN_UNEXPECTED_TOKEN,
                CC71_GlobalCurrentLine,
                columnAux,
                "Expected a constant expression after 'case'."
            );

            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "labeled_statement()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenColon) {
            CC71_ReportExpectedTokenError(TokenColon, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "labeled_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!statement()) {
            CC71_SilentMode = 0;

            CC71_ReportError(
                CC71_ERR_SYN_UNEXPECTED_TOKEN,
                CC71_GlobalCurrentLine,
                columnAux,
                "Expected a statement after 'case constant_expression:'."
            );

            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "labeled_statement()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "labeled_statement()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenDefault) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenColon) {
            CC71_ReportExpectedTokenError(TokenColon, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "labeled_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!statement()) {
            CC71_SilentMode = 0;

            CC71_ReportError(
                CC71_ERR_SYN_UNEXPECTED_TOKEN,
                CC71_GlobalCurrentLine,
                columnAux,
                "Expected a statement after 'default:'."
            );

            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "labeled_statement()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "labeled_statement()");
        return PARSE_SUCCESS;
    }

    CC71_ReportError(
        CC71_ERR_SYN_UNEXPECTED_TOKEN,
        CC71_GlobalCurrentLine,
        columnAux,
        "Expected TokenIdentifier, TokenCase or TokenDefault but found '%s'.",
        CC71_TokenToString(CC71_GlobalTokenNumber)
    );

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "labeled_statement()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


int expression_statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "expression_statement()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenSemicolon) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "expression_statement()");
        return PARSE_SUCCESS;
    }

    if (expression()) {
        if (CC71_GlobalTokenNumber == TokenSemicolon) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "expression_statement()");
            return PARSE_SUCCESS;
        }

        // TODO: VERIFICAR
        CC71_ReportExpectedTokenError(TokenSemicolon, FORCE_ERROR_THROW);
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "expression_statement()");
        return PARSE_FAIL;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "expression_statement()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int selection_statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "selection_statement()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenIf) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenOpenParentheses) {
            CC71_ReportExpectedTokenError(TokenOpenParentheses, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "selection_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!expression()) {
            CC71_SilentMode = 0;

            CC71_ReportError(
                CC71_ERR_SYN_UNEXPECTED_TOKEN,
                CC71_GlobalCurrentLine,
                columnAux,
                "Expected expression after '('."
            );

            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "selection_statement()");
            return PARSE_FAIL;
        }
        
        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportExpectedTokenError(TokenCloseParentheses, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "selection_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!statement()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "selection_statement()");
            return PARSE_FAIL;
        }

        if (!selection_statement_else_opt()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "selection_statement()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "selection_statement()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenSwitch) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenOpenParentheses) {
            CC71_ReportExpectedTokenError(TokenOpenParentheses);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "selection_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!expression()) {
            CC71_SilentMode = 0;

            CC71_ReportError(
                CC71_ERR_SYN_UNEXPECTED_TOKEN,
                CC71_GlobalCurrentLine,
                columnAux,
                "Expected expression after '('."
            );

            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "selection_statement()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportExpectedTokenError(TokenCloseParentheses, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "selection_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!statement()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "selection_statement()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "selection_statement()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "selection_statement()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int selection_statement_else_opt() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "selection_statement_else_opt()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenElse) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!statement()) {
            CC71_SilentMode = 0;

            CC71_ReportError(
                CC71_ERR_SYN_UNEXPECTED_TOKEN,
                CC71_GlobalCurrentLine,
                columnAux,
                "Expected statement after 'else'."
            );

            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "selection_statement_else_opt()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "selection_statement_else_opt()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "selection_statement_else_opt()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int iteration_statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "iteration_statement()");

    zenith_backtracking_start();
    
    if (CC71_GlobalTokenNumber == TokenWhile) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenOpenParentheses) {
            CC71_ReportExpectedTokenError(TokenOpenParentheses, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!expression()) {
            CC71_SilentMode = 0;

            CC71_ReportError(
                CC71_ERR_SYN_UNEXPECTED_TOKEN,
                CC71_GlobalCurrentLine,
                columnAux,
                "Expected expression after '('."
            );

            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportExpectedTokenError(TokenCloseParentheses, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!statement()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "iteration_statement()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenDo) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        
        if (!statement()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenWhile) {
            CC71_ReportExpectedTokenError(TokenWhile, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenOpenParentheses) {
            CC71_ReportExpectedTokenError(TokenOpenParentheses, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        
        if (!expression()) {
            CC71_SilentMode = 0;
            
            CC71_ReportError(
                CC71_ERR_SYN_UNEXPECTED_TOKEN,
                CC71_GlobalCurrentLine,
                columnAux,
                "Expected expression after '('."
            );

            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportExpectedTokenError(TokenCloseParentheses, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenSemicolon) {
            CC71_ReportExpectedTokenError(TokenSemicolon, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "iteration_statement()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenFor) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenOpenParentheses) {
            CC71_ReportExpectedTokenError(TokenOpenParentheses, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!expression_statement()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        if (!expression_statement()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();

            if (!statement()) {
                zenith_backtracking_restore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
                return PARSE_FAIL;
            }

            zenith_backtracking_end();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "iteration_statement()");
            return PARSE_SUCCESS;
        }

        if (!expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportExpectedTokenError(TokenCloseParentheses, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!statement()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "iteration_statement()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "iteration_statement()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int jump_statement() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "jump_statement()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenGoto) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenIdentifier) {
            CC71_ReportExpectedTokenError(TokenIdentifier, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "jump_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenSemicolon) {
            CC71_ReportExpectedTokenError(TokenSemicolon, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "jump_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "jump_statement()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenContinue) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenSemicolon) {
            CC71_ReportExpectedTokenError(TokenSemicolon, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "jump_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "jump_statement()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenBreak) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenSemicolon) {
            CC71_ReportExpectedTokenError(TokenSemicolon, FORCE_ERROR_THROW);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "jump_statement()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "jump_statement()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenReturn) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber == TokenSemicolon) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "jump_statement()");
            return PARSE_SUCCESS;
        }

        if (expression()) {
            if (CC71_GlobalTokenNumber != TokenSemicolon) {
                CC71_ReportExpectedTokenError(TokenSemicolon, FORCE_ERROR_THROW);
                zenith_backtracking_restore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "jump_statement()");
                return PARSE_FAIL;
            }
            
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "jump_statement()");
            return PARSE_SUCCESS;
        }

        CC71_ReportExpectedTokenError(TokenSemicolon, FORCE_ERROR_THROW);
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "jump_statement()");
        return PARSE_FAIL;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "jump_statement()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "expression()");

    zenith_backtracking_start();

    if (!assignment_expression()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "expression()");
        return PARSE_FAIL;
    }

    if (!expression_sequence()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "expression()");
        return PARSE_FAIL;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int expression_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "expression_sequence()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!assignment_expression()) {
            //CC71_ReportError(CC71_ERR_SYN_UNEXPECTED_TOKEN, CC71_GlobalCurrentLine, columnAux, "Expected assignment_expression after ','.");
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "expression_sequence()");
            return PARSE_FAIL;
        }

        if (!expression_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "expression_sequence()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "expression_sequence()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_end();
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


// OK
int argument_expression_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "argument_expression_list()");

    if (!assignment_expression()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "argument_expression_list()");
        return PARSE_FAIL;
    }

    if (!argument_expression_sequence()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "argument_expression_list()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "argument_expression_list()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int argument_expression_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "argument_expression_sequence()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenComma) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!assignment_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "argument_expression_sequence()");
            return PARSE_FAIL;
        }

        if (!argument_expression_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "argument_expression_sequence()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "argument_expression_sequence()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "argument_expression_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int assignment_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "assignment_expression()");

    zenith_backtracking_start();

    if (unary_expression()) {
        if (assignment_operator()) {
            if (assignment_expression()) {
                zenith_backtracking_end();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "assignment_expression()");
                return PARSE_SUCCESS;
            } else {
                CC71_SilentMode = 0;

                CC71_ReportError(
                    CC71_ERR_SYN_UNEXPECTED_TOKEN,
                    CC71_GlobalCurrentLine,
                    columnAux,
                    "Expected expression before ';'.",
                    CC71_TokenToString(CC71_GlobalTokenNumber)
                );
            }
        }
    }

    zenith_backtracking_restore();
    zenith_backtracking_start();

    if (conditional_expression()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "assignment_expression()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "assignment_expression()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


int conditional_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "conditional_expression()");

    zenith_backtracking_start();

    if (!logical_or_expression()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "conditional_expression()");     
        return PARSE_FAIL;
    }

    if (!conditional_expression_suffix()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "conditional_expression()");
        return PARSE_FAIL;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "conditional_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int conditional_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "conditional_expression_suffix()");

    zenith_backtracking_start();
    
    if (CC71_GlobalTokenNumber == TokenQuestionMark) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "conditional_expression_suffix()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenColon) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "conditional_expression_suffix()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!conditional_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "conditional_expression_suffix()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "conditional_expression_suffix()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
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
        zenith_get_token();

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
    
    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenLogicalAnd) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!inclusive_or_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "logical_and_expression_suffix()");
            return PARSE_FAIL;
        }

        if (!logical_and_expression_suffix()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "logical_and_expression_suffix()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "logical_and_expression_suffix()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "logical_and_expression_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int inclusive_or_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "inclusive_or_expression()");
    
    if (!exclusive_or_expression()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "inclusive_or_expression()");
        return PARSE_FAIL;
    }

    if (!inclusive_or_expression_suffix()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "inclusive_or_expression()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "inclusive_or_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int inclusive_or_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "inclusive_or_expression_suffix()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenBitwiseOr) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!exclusive_or_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "inclusive_or_expression_suffix()");
            return PARSE_FAIL;
        }

        if (!inclusive_or_expression_suffix()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "inclusive_or_expression_suffix()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "inclusive_or_expression_suffix()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "inclusive_or_expression_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int exclusive_or_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "exclusive_or_expression()");

    if (!and_expression()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "exclusive_or_expression()");
        return PARSE_FAIL;
    }
    
    if (!exclusive_or_expression_suffix()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "exclusive_or_expression()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "exclusive_or_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int exclusive_or_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "exclusive_or_expression_suffix()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenBitwiseXor) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        
        if (!and_expression()) {
            BACKTRACK_FAIL();
        }

        if (!exclusive_or_expression_suffix()) {
            BACKTRACK_FAIL();
        }

        BACKTRACK_END();
        return PARSE_SUCCESS;
    }
    
    BACKTRACK_END();
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int and_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "and_expression()");

    if (!equality_expression()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "and_expression()");
        return PARSE_FAIL;
    }

    if (!and_expression_suffix()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "and_expression()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "and_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int and_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "and_expression_suffix()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenBitwiseAnd_AddressOf) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!equality_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "and_expression_suffix()");
            return PARSE_FAIL;
        }

        if (!and_expression_suffix()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "and_expression_suffix()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "and_expression_suffix()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "and_expression_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int equality_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "equality_expression()");

    zenith_backtracking_start();

    if (!relational_expression()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "equality_expression()");
        return PARSE_FAIL;
    }

    if (!equality_expression_suffix()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "equality_expression()");
        return PARSE_FAIL;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "equality_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int equality_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "equality_expression_suffix()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenEqual || CC71_GlobalTokenNumber == TokenNotEqual) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!relational_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "equality_expression_suffix()");
            return PARSE_FAIL;
        }

        if (!equality_expression_suffix()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "equality_expression_suffix()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "equality_expression_suffix()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "equality_expression_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int relational_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "relational_expression()");

    zenith_backtracking_start();

    if (!shift_expression()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "relational_expression()");
        return PARSE_FAIL;
    }

    if (!relational_expression_suffix()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "relational_expression()");
        return PARSE_FAIL;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "relational_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int relational_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "relational_expression_suffix()");
    
    zenith_backtracking_start();

    switch (CC71_GlobalTokenNumber) {
        case TokenLessThan:
        case TokenGreaterThan:
        case TokenLessEqual:
        case TokenGreaterEqual:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();

            if (!shift_expression()) {
                zenith_backtracking_restore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "relational_expression_suffix()");
                return PARSE_FAIL;
            }

            if (!relational_expression_suffix()) {
                zenith_backtracking_restore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "relational_expression_suffix()");
                return PARSE_FAIL;
            }

            zenith_backtracking_end();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "relational_expression_suffix()");
            return PARSE_SUCCESS;
        
        default:
            zenith_backtracking_end();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "relational_expression_suffix()");
            return PARSE_SUCCESS;
    }
}


/////////////////////////////////////////////////////////////////////////////


// OK
int shift_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "shift_expression()");

    zenith_backtracking_start();

    if (!additive_expression()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "shift_expression()");
        return PARSE_FAIL;
    }

    if (!shift_expression_suffix()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "shift_expression()");
        return PARSE_FAIL;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "shift_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int shift_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "shift_expression_suffix()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenLeftShift || CC71_GlobalTokenNumber == TokenRightShift) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!additive_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "shift_expression_suffix()");
            return PARSE_FAIL;
        }

        if (!shift_expression_suffix()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "shift_expression_suffix()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "shift_expression_suffix()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "shift_expression_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int additive_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "additive_expression()");

    zenith_backtracking_start();

    if (!multiplicative_expression()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "additive_expression()");
        return PARSE_FAIL;
    }

    if (!additive_expression_suffix()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "additive_expression()");
        return PARSE_FAIL;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "additive_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


//
int additive_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "additive_expression_suffix()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenPlus || CC71_GlobalTokenNumber == TokenMinus) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!multiplicative_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "additive_expression_suffix()");
            return PARSE_FAIL;
        }

        if (!additive_expression_suffix()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "additive_expression_suffix()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "additive_expression_suffix()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "additive_expression_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


int multiplicative_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "multiplicative_expression()");

    zenith_backtracking_start();
    
    if (!cast_expression()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "multiplicative_expression()");
        return PARSE_FAIL;
    }

    if (!multiplicative_expression_suffix()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "multiplicative_expression()");
        return PARSE_FAIL;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "multiplicative_expression()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int multiplicative_expression_suffix() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "multiplicative_expression_suffix()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenAsterisk || CC71_GlobalTokenNumber == TokenDivision || CC71_GlobalTokenNumber == TokenMod) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!cast_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "multiplicative_expression_suffix()");
            return PARSE_FAIL;
        }

        if (!multiplicative_expression_suffix()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "multiplicative_expression_suffix()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "multiplicative_expression_suffix()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "multiplicative_expression_suffix()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int cast_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "cast_expression()");

    if (unary_expression()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "cast_expression()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!type_name()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "cast_expression()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "cast_expression()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!cast_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "cast_expression()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "cast_expression()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "cast_expression()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int unary_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "unary_expression()");

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenIncrement) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!unary_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "unary_expression() (prefix ++)");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenDecrement) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!unary_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "unary_expression() (prefix --)");
        return PARSE_SUCCESS;
    }

    if (unary_operator()) {
        if (!cast_expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "unary_expression() (unary_operator)");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenSizeof) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (unary_expression()) {
            zenith_backtracking_end();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "unary_expression() (sizeof expr)");
            return PARSE_SUCCESS;
        }

        if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();

            if (!type_name()) {
                zenith_backtracking_restore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
                return PARSE_FAIL;
            }

            if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                zenith_backtracking_restore();
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
                return PARSE_FAIL;
            }

            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();

            zenith_backtracking_end();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "unary_expression() (sizeof type)");
            return PARSE_SUCCESS;
        }

        // TEST OK
        CC71_ReportExpectedTokenError(TokenOpenParentheses);
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
        return PARSE_FAIL;
    }

    if (postfix_expression()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "unary_expression() (postfix)");
        return PARSE_SUCCESS;
    }

    // TEST OK PARA SIZEOF
    //CC71_ReportExpectedTokenError(TokenCloseParentheses);
    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "unary_expression()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int postfix_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "postfix_expression()");

    zenith_backtracking_start();

    if (!primary_expression()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression()");
        return PARSE_FAIL;
    }

    if (!postfix_expression_sequence()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression()");
        return PARSE_FAIL;
    }

    zenith_backtracking_end();
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

    zenith_backtracking_start();

    if (CC71_GlobalTokenNumber == TokenOpenBracket) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (!expression()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenCloseBracket) {
            CC71_ReportExpectedTokenError(TokenCloseBracket);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();
            zenith_backtracking_end();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
            return PARSE_SUCCESS;
        }

        if (!argument_expression_list()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
            return PARSE_FAIL;
        }

        if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
            CC71_ReportExpectedTokenError(TokenCloseParentheses);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenDot) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenIdentifier) {
            CC71_ReportExpectedTokenError(TokenIdentifier);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
            return PARSE_FAIL;
        }

        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenArrow) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();

        if (CC71_GlobalTokenNumber != TokenIdentifier) {
            CC71_ReportExpectedTokenError(TokenIdentifier);
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
            return PARSE_FAIL;
        }

        zenith_get_token();
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenIncrement) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
        return PARSE_SUCCESS;
    }

    if (CC71_GlobalTokenNumber == TokenDecrement) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
        zenith_get_token();
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "postfix_expression_suffix()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "postfix_expression_suffix()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int primary_expression() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "primary_expression()");
    
    switch(CC71_GlobalTokenNumber) {
        case TokenIdentifier:
        case TokenIntConst:
        case TokenFloatConst:
        case TokenCharConst:
        case TokenString:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "primary_expression()");
            return PARSE_SUCCESS;

        case TokenOpenParentheses:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();

            if (!expression()) {
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "primary_expression()");
                return PARSE_FAIL;
            }

            if (CC71_GlobalTokenNumber != TokenCloseParentheses) {
                //CC71_ReportExpectedTokenError(TokenCloseParentheses);
                CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "primary_expression()");
                return PARSE_FAIL;
            }

            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "primary_expression()");
            return PARSE_SUCCESS;

        default:
            //CC71_ReportExpectedTokenError(TokenIdentifier);
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

    if (composite_type_specifier()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "type_specifier()");
        return PARSE_SUCCESS;
    }
    
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "type_specifier()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int primitive_type_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "primitive_type_specifier()");
    
    switch (CC71_GlobalTokenNumber) {
        case TokenVoid:
        case TokenSigned:
        case TokenUnsigned:
        case TokenChar:
        case TokenShort:
        case TokenInt:
        case TokenLong:
        case TokenFloat:
        case TokenDouble:
        case TokenBool:
        case TokenComplex:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "primitive_type_specifier()");
            return PARSE_SUCCESS;
        
        // TODO: Improve this issue in the future in the semantic analyzer.
        /*case TokenIdentifier:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "primitive_type_specifier()");
            return PARSE_SUCCESS;
        */

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

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "composite_type_specifierS()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int storage_specifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "storage_specifier()");
    
    switch (CC71_GlobalTokenNumber) {
        case TokenAuto:
        case TokenExtern:
        case TokenRegister:
        case TokenStatic:
        case TokenTypedef:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ACCEPTED_TOKEN, NULL, CC71_GlobalTokenNumber, lex);
            zenith_get_token();
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
            zenith_get_token();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "type_qualifier()");
            return PARSE_SUCCESS;
        default:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "type_qualifier()");
            return PARSE_FAIL;
    }
}


/////////////////////////////////////////////////////////////////////////////


// OK
int type_qualifier_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "type_qualifier_list()");

    if (!type_qualifier()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "type_qualifier_list()");
        return PARSE_FAIL;
    }

    if (!type_qualifier_list_sequence()) {
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "type_qualifier_list()");
        return PARSE_FAIL;
    }

    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "type_qualifier_list()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int type_qualifier_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "type_qualifier_list_sequence()");

    zenith_backtracking_start();

    if (type_qualifier()) {
        if (!type_qualifier_list_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "type_qualifier_list_sequence()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "type_qualifier_list_sequence()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "type_qualifier_list_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int specifier_qualifier_list() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "specifier_qualifier_list()");
    
    zenith_backtracking_start();

    if (!specifier_qualifier()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "specifier_qualifier_list()");
        return PARSE_FAIL;
    }

    if (!specifier_qualifier_list_sequence()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "specifier_qualifier_list()");
        return PARSE_FAIL;
    }

    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "specifier_qualifier_list()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int specifier_qualifier_list_sequence() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "specifier_qualifier_list_sequence()");

    zenith_backtracking_start();

    if (specifier_qualifier()) {
        if (!specifier_qualifier_list_sequence()) {
            zenith_backtracking_restore();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "specifier_qualifier_list_sequence()");
            return PARSE_FAIL;
        }

        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "specifier_qualifier_list_sequence()");
        return PARSE_SUCCESS;
    }
    
    zenith_backtracking_end();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "specifier_qualifier_list_sequence()");
    return PARSE_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int specifier_qualifier() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "specifier_qualifier()");

    zenith_backtracking_start();
    
    if (type_specifier()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "specifier_qualifier()");
        return PARSE_SUCCESS;
    }

    if (type_qualifier()) {
        zenith_backtracking_end();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "specifier_qualifier()");
        return PARSE_SUCCESS;
    }

    zenith_backtracking_restore();
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "specifier_qualifier()");
    return PARSE_FAIL;
}


/////////////////////////////////////////////////////////////////////////////


// OK
int type_name() {
    CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_ENTER_FUNCTION, NULL, "type_name()");

    zenith_backtracking_start();
    
    if (!specifier_qualifier_list()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "type_name()");
        return PARSE_FAIL;
    }

    if (!type_name_suffix()) {
        zenith_backtracking_restore();
        CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "type_name()");
        return PARSE_FAIL;
    }

    zenith_backtracking_end();
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
            zenith_get_token();
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
            zenith_get_token();
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_SUCCESS, NULL, "assignment_operator()");
            return PARSE_SUCCESS;
        default:
            CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_EXIT_FAILURE, NULL, "assignment_operator()");
            return PARSE_FAIL;
    }
}
