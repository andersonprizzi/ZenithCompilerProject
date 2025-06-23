/////////////////////////////////////////////////////////////////////////////
// LIBRARY INCLUDES                                                        //
/////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "logger.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"

/////////////////////////////////////////////////////////////////////////////
// PARSER: DECLARATION AND TYPE ANALYSIS                                   //
/////////////////////////////////////////////////////////////////////////////


int storage_class_specifier() {

}


/////////////////////////////////////////////////////////////////////////////


int primitive_type_specifier() {

}


/////////////////////////////////////////////////////////////////////////////


int composite_type_specifier() {

}


/////////////////////////////////////////////////////////////////////////////


int type_qualifier() {

}



/////////////////////////////////////////////////////////////////////////////
// PARSER                                                                  //
/////////////////////////////////////////////////////////////////////////////







/////////////////////////////////////////////////////////////////////////////


int translation_unit() {
    printf("[translation_unit()]\n");

    if (external_declaration()) {
        return TRUE;
    } else if (translation_unit()) {
        if (external_declaration()) {
            return TRUE;
        }
    }
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////


int external_declaration() {
    printf("[external_declaration()]\n");

    if (function_definition()) {
        return TRUE;
    } else if (declaration()) {
        return TRUE;
    }
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////


int function_definition() {

}


/////////////////////////////////////////////////////////////////////////////


int declaration() {

}


/////////////////////////////////////////////////////////////////////////////


int declaration_specifiers() {

}


/////////////////////////////////////////////////////////////////////////////


int expression() {
    if (T()) {
        if (E_prime()) {
            return TRUE;
        }
    }
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////


int primary_expression() {
    if (tokenNumber == TokenIdentifier ||
        tokenNumber == TokenIntConst ||
        tokenNumber == TokenFloatConst ||
        tokenNumber == TokenString
    ) {
        getToken();
        CC71_LogMessage(LOG_INFO, "Recognized a primary expression.");
        return TRUE;

    } else if (tokenNumber == TokenOpenParentheses) {
        getToken();
        if (expression()) {
            if (tokenNumber == TokenCloseParentheses) {
                getToken();
                return TRUE;
            } else {
                AC71_ReportError(AC71_ERR_SYN_EXPECTED_CLOSE_PAREN, line, columnAux);
                return FALSE;
            }
        } else {
            AC71_ReportError(AC71_ERR_SYN_UNEXPECTED_TOKEN, line, columnAux);
            return FALSE;
        }
    }

    AC71_ReportError(AC71_ERR_SYN_UNEXPECTED_TOKEN, line, columnAux);
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////


// ()  []  ->  .
int postfix_expression() {
    if (primary_expression()) {
        return TRUE;

    } else if (tokenNumber == TokenOpenBracket) {
        getToken();
        if (expression()) {
            if (tokenNumber == TokenCloseBracket) {
                getToken();
                return TRUE;
            } else {
                AC71_ReportError(AC71_ERR_SYN_UNMATCHED_BRACKET, line, columnAux);
                return FALSE;
            }
        } else {
            AC71_ReportError(AC71_ERR_SYN_UNEXPECTED_TOKEN, line, columnAux);
            return FALSE;
        }

    } else if (tokenNumber == TokenOpenParentheses) {
        getToken();
        if (tokenNumber == TokenCloseParentheses) {
            getToken();
        } else {
            /*if (!argument_expression_list()) {
                return FALSE;
            }
            if (tokenNumber != TokenCloseParentheses) {
                AC71_ReportError(AC71_ERR_SYN_EXPECTED_CLOSE_PAREN, line, columnAux);
                return FALSE;
            }
            getToken(); // Consome ')'
            printf("Reconheceu chamada de fun��o com argumentos\n");*/
        }

    // TODO: FALTA FAZER O TOKEN DOT (.)
    } else if (tokenNumber == TokenArrow) {
        getToken();
        if (tokenNumber == TokenIdentifier) {
            getToken();
            return TRUE;
        } else {
            AC71_ReportError(AC71_ERR_SYN_UNEXPECTED_TOKEN, line, columnAux);
            return FALSE;
        }

    } else if (tokenNumber == TokenIncrement) {
        getToken();
        return TRUE;

    } else if (tokenNumber == TokenDecrement) {
        getToken();
        return TRUE;
    }

    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////


int unary_expression() {
    if (postfix_expression()) {
        return TRUE;

    } else if (tokenNumber == TokenIncrement) {
        getToken();
        if (unary_expression()) {
            return TRUE;
        } else {
            AC71_ReportError(AC71_ERR_SYN_UNEXPECTED_TOKEN, line, columnAux);
            return FALSE;
        }

    } else if (tokenNumber == TokenDecrement) {
        getToken();
        if (unary_expression()) {
            return TRUE;
        } else {
            AC71_ReportError(AC71_ERR_SYN_UNEXPECTED_TOKEN, line, columnAux);
            return FALSE;
        }

    } else if (unary_operator()) {
        getToken();
        if (cast_expression()) {
            return TRUE;
        } else {
            AC71_ReportError(AC71_ERR_SYN_UNEXPECTED_TOKEN, line, columnAux);
            return FALSE;
        }
    }
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////


int cast_expression() {
    if (unary_expression()) {
        return TRUE;
    }
    // TODO: FAZER O CAST COM PARENTESES AQUI.
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////


int multiplicative_expression() {

}


/////////////////////////////////////////////////////////////////////////////

int additive_expression() {

}


/////////////////////////////////////////////////////////////////////////////


int shift_expression() {


}


/////////////////////////////////////////////////////////////////////////////


int relational_expression() {

}


/////////////////////////////////////////////////////////////////////////////


int equality_expression() {

}


/////////////////////////////////////////////////////////////////////////////


int and_expression() {

}


/////////////////////////////////////////////////////////////////////////////


int exclusive_or_expression() {

}


/////////////////////////////////////////////////////////////////////////////

int inclusive_or_expression() {

}


/////////////////////////////////////////////////////////////////////////////


int logical_and_expression() {

}


/////////////////////////////////////////////////////////////////////////////


int logical_or_expression() {

}


/////////////////////////////////////////////////////////////////////////////


int conditional_expression() {

}


/////////////////////////////////////////////////////////////////////////////


int assignment_expression() {

}




/////////////////////////////////////////////////////////////////////////////

int T() {
    if (tokenNumber == TokenIdentifier) {
        getToken();
        CC71_LogMessage(LOG_INFO, "Recognized an identifier.");
        return TRUE;
    }
    return FALSE;
}


int A() {
    marcaPosToken();
    if (tokenNumber == TokenIdentifier) {
        getToken();
        if (A()) return 1;

    }
}


/////////////////////////////////////////////////////////////////////////////


int E_prime() {
    if (tokenNumber == TokenPlus) {
        getToken();
        if (T()) {
            if (E_prime()) {
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    } else {
        return TRUE;
    }
}


/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////


int unary_operator() {
    if (tokenNumber == TokenBitwiseAnd_AddressOf ||
        tokenNumber == TokenAsterisk ||
        tokenNumber == TokenPlus ||
        tokenNumber == TokenMinus ||
        tokenNumber == TokenBitwiseNot ||
        tokenNumber == TokenLogicalNot
    ) {
        getToken();
        CC71_LogMessage(LOG_INFO, "Recognized a unary operator.");
        return TRUE;
    }
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////


/*int expression() {
    if (assignment_expression()) {
        return TRUE;
    } else if (expression()) {
        if (tokenNumber == TokenComma) {
            if (assignment_expression()) {
                return TRUE;
            }
        }
    }
    return FALSE;
}*/


/////////////////////////////////////////////////////////////////////////////







/////////////////////////////////////////////////////////////////////////////


int statement() {
    if (tokenNumber == TokenIf) {
        if (selection_statement()) {
            return TRUE;
        }
    } else if (tokenNumber == TokenOpenBrace) {
        if (compound_statement()) {
            return TRUE;
        }
    } else {
        if (expression_statement()) {
            return TRUE;
        }
    }
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////


int expression_statement() {
    if(expression()) {
        if(tokenNumber == TokenSemicolon) {
                getToken();
            return TRUE;
        }
    } else {
        if(tokenNumber == TokenSemicolon) {
            getToken();
            return TRUE;
        }
    }
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////


int selection_statement() {
    if (tokenNumber == TokenIf) {
        getToken();
        CC71_LogMessage(LOG_INFO, "Recognized 'if'.");

        if (tokenNumber == TokenOpenParentheses) {
            getToken();
            CC71_LogMessage(LOG_INFO, "Recognized '('.");

            if (expression()) {
                CC71_LogMessage(LOG_INFO, "Recognized an expression.");

                if (tokenNumber == TokenCloseParentheses) {
                    getToken();
                    CC71_LogMessage(LOG_INFO, "Recognized ')'.");

                    if(statement()) {
                        CC71_LogMessage(LOG_INFO, "Recognized a statement.");
                        return TRUE;
                    } else {
                        AC71_ReportError(AC71_ERR_SYN_EXPECTED_CLOSE_PAREN, line, columnAux);
                        return FALSE;
                    }
                }
            }
        } else {
            AC71_ReportError(AC71_ERR_SYN_EXPECTED_OPEN_PAREN, line, columnAux);
            return FALSE;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////


int compound_statement() {
    if (tokenNumber == TokenOpenBrace) {
        getToken();
        CC71_LogMessage(LOG_INFO, "Recognized '{'.");

        statement();

        if (tokenNumber == TokenCloseBrace) {
            getToken();
            CC71_LogMessage(LOG_INFO, "Recognized '}'.");
            return TRUE;
        }
    }
    return FALSE;
}










