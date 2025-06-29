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
// PARSER: FUNDAMENTAL UNIT OF ANALYSIS                                    //
/////////////////////////////////////////////////////////////////////////////

int CC71_ParseTranslationUnit() {
    while (1) {
        if (!external_declaration()) break;
    }
    return 1;
}



/////////////////////////////////////////////////////////////////////////////
// PARSER: DECLARATION AND TYPE ANALYSIS                                   //
/////////////////////////////////////////////////////////////////////////////


int CC71_ParseStorageClassSpecifier() {
    switch (CC71_GlobalTokenNumber) {
        case TokenAuto:
        case TokenExtern:
        case TokenRegister:
        case TokenStatic:
        case TokenTypedef:
            CC71_GetToken();
            return 1;
        default:
            return 0;
    }
}


/////////////////////////////////////////////////////////////////////////////


int CC71_ParsePrimitiveTypeSpecifier() {
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
            CC71_GetToken();
            return 1;
        default:
            return 0;
    }
}


/////////////////////////////////////////////////////////////////////////////


int CC71_ParseCompositeTypeSpecifier() {
    switch (CC71_GlobalTokenNumber) {
        case TokenStruct:
        case TokenUnion:
        case TokenEnum:
            CC71_GetToken();
            return 1;
        default:
            return 0;
    }
}


/////////////////////////////////////////////////////////////////////////////


int CC71_ParseTypeQualifier() {
    switch (CC71_GlobalTokenNumber) {
        case TokenConst:
        case TokenVolatile:
        case TokenRestrict:
        case TokenAtomic:
            CC71_GetToken();
            return 1;
        default:
            return 0;
    }
}


/////////////////////////////////////////////////////////////////////////////
// PARSER                                                                  //
/////////////////////////////////////////////////////////////////////////////






// Tenta consumir pelo menos um storage_class_specifier, type_specifier ou type_qualifier
int declaration_specifiers() {
    int consumed = 0;
    int progress;

    do {
        progress = 0;
        if (CC71_ParseStorageClassSpecifier()) {
            consumed = 1;
            progress = 1;
            continue;
        }
        if (CC71_ParsePrimitiveTypeSpecifier()) {
            consumed = 1;
            progress = 1;
            continue;
        }
        if (CC71_ParseCompositeTypeSpecifier()) {
            consumed = 1;
            progress = 1;
            continue;
        }
        if (CC71_ParseTypeQualifier()) {
            consumed = 1;
            progress = 1;
            continue;
        }
    } while (progress);

    return consumed;
}

// Declarator simples: só identificador
int declarator() {
    if (CC71_GlobalTokenNumber == TokenIdentifier) {
        CC71_GetToken();
        return 1;
    }
    return 0;
}

// Compound statement: '{' ... '}'
int compound_statement() {
    if (CC71_GlobalTokenNumber != TokenLeftBrace) {
        return 0;
    }
    CC71_GetToken(); // consome '{'

    // Para simplificar, só consumimos tokens até encontrar '}'
    // Pode ser expandido depois para reconhecer statements
    while (CC71_GlobalTokenNumber != TokenRightBrace && CC71_GlobalTokenNumber != TokenEOF) {
        CC71_GetToken();
    }

    if (CC71_GlobalTokenNumber != TokenRightBrace) {
        CC71_ParserError("Expected '}' at end of compound statement");
        return 0;
    }
    CC71_GetToken(); // consome '}'

    return 1;
}

// external_declaration: declaração ou função
int external_declaration() {
    // 1. declaraton_specifiers obrigatórios
    if (!declaration_specifiers()) {
        return 0;
    }

    // 2. declarator obrigatorio (nome da variável ou função)
    if (!declarator()) {
        CC71_ParserError("Expected declarator");
        return 0;
    }

    // 3. pode ser uma função (parênteses) ou declaração (ponto e vírgula)

    if (CC71_GlobalTokenNumber == TokenLeftParen) {
        // Função: reconhecimento simplificado de parâmetros ignorados
        CC71_GetToken(); // consome '('

        // Para simplificar, ignoramos lista de parâmetros e só consumimos até fechar parênteses
        int paren_count = 1;
        while (paren_count > 0 && CC71_GlobalTokenNumber != TokenEOF) {
            if (CC71_GlobalTokenNumber == TokenLeftParen) paren_count++;
            else if (CC71_GlobalTokenNumber == TokenRightParen) paren_count--;
            CC71_GetToken();
        }

        if (paren_count != 0) {
            CC71_ParserError("Unmatched parentheses in function parameter list");
            return 0;
        }

        // Agora espera o corpo da função (compound statement)
        if (!compound_statement()) {
            CC71_ParserError("Expected function body");
            return 0;
        }

        return 1;
    }
    else if (CC71_GlobalTokenNumber == TokenSemicolon) {
        // Declaração simples
        CC71_GetToken();
        return 1;
    }
    else {
        CC71_ParserError("Expected '(' for function or ';' for declaration");
        return 0;
    }
}






/////////////////////////////////////////////////////////////////////////////
// PARSER                                                                  //
/////////////////////////////////////////////////////////////////////////////







/////////////////////////////////////////////////////////////////////////////


int CC71_ParseTranslationUnit() {
    printf("[translation_unit()]\n");

    if (external_declaration()) {
        return TRUE;
    } else if (CC71_ParseTranslationUnit()) {
        if (external_declaration()) {
            return TRUE;
        }
    }
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////


int external_declaration() {
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

/*
int primary_expression() {
    if (CC71_GlobalTokenNumber == TokenIdentifier ||
        CC71_GlobalTokenNumber == TokenIntConst ||
        CC71_GlobalTokenNumber == TokenFloatConst ||
        CC71_GlobalTokenNumber == TokenString
    ) {
        CC71_GetToken();
        CC71_LogMessage(CC71_LOG_INFO, "Recognized a primary expression.");
        return TRUE;

    } else if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_GetToken();
        if (expression()) {
            if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
                CC71_GetToken();
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
*/

/////////////////////////////////////////////////////////////////////////////


// ()  []  ->  .
/*
int postfix_expression() {
    if (primary_expression()) {
        return TRUE;

    } else if (CC71_GlobalTokenNumber == TokenOpenBracket) {
        CC71_GetToken();
        if (expression()) {
            if (CC71_GlobalTokenNumber == TokenCloseBracket) {
                CC71_GetToken();
                return TRUE;
            } else {
                AC71_ReportError(AC71_ERR_SYN_UNMATCHED_BRACKET, line, columnAux);
                return FALSE;
            }
        } else {
            AC71_ReportError(AC71_ERR_SYN_UNEXPECTED_TOKEN, line, columnAux);
            return FALSE;
        }

    } else if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
        CC71_GetToken();
        if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
            CC71_GetToken();
        } else {

        }

    // TODO: FALTA FAZER O TOKEN DOT (.)
    } else if (CC71_GlobalTokenNumber == TokenArrow) {
        CC71_GetToken();
        if (CC71_GlobalTokenNumber == TokenIdentifier) {
            CC71_GetToken();
            return TRUE;
        } else {
            AC71_ReportError(AC71_ERR_SYN_UNEXPECTED_TOKEN, line, columnAux);
            return FALSE;
        }

    } else if (CC71_GlobalTokenNumber == TokenIncrement) {
        CC71_GetToken();
        return TRUE;

    } else if (CC71_GlobalTokenNumber == TokenDecrement) {
        CC71_GetToken();
        return TRUE;
    }

    return FALSE;
}
*/

/////////////////////////////////////////////////////////////////////////////

/*
int unary_expression() {
    if (postfix_expression()) {
        return TRUE;

    } else if (CC71_GlobalTokenNumber == TokenIncrement) {
        CC71_GetToken();
        if (unary_expression()) {
            return TRUE;
        } else {
            AC71_ReportError(AC71_ERR_SYN_UNEXPECTED_TOKEN, line, columnAux);
            return FALSE;
        }

    } else if (CC71_GlobalTokenNumber == TokenDecrement) {
        CC71_GetToken();
        if (unary_expression()) {
            return TRUE;
        } else {
            AC71_ReportError(AC71_ERR_SYN_UNEXPECTED_TOKEN, line, columnAux);
            return FALSE;
        }

    } else if (unary_operator()) {
        CC71_GetToken();
        if (cast_expression()) {
            return TRUE;
        } else {
            AC71_ReportError(AC71_ERR_SYN_UNEXPECTED_TOKEN, line, columnAux);
            return FALSE;
        }
    }
    return FALSE;
}
*/

/////////////////////////////////////////////////////////////////////////////


int cast_expression() {
    /*if (unary_expression()) {
        return TRUE;
    }*/
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
    if (CC71_GlobalTokenNumber == TokenIdentifier) {
        CC71_GetToken();
        CC71_LogMessage(CC71_LOG_INFO, "Recognized an identifier.");
        return TRUE;
    }
    return FALSE;
}


int A() {
    marcaPosToken();
    if (CC71_GlobalTokenNumber == TokenIdentifier) {
        CC71_GetToken();
        if (A()) return 1;

    }
}


/////////////////////////////////////////////////////////////////////////////


int E_prime() {
    if (CC71_GlobalTokenNumber == TokenPlus) {
        CC71_GetToken();
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
    if (CC71_GlobalTokenNumber == TokenBitwiseAnd_AddressOf ||
        CC71_GlobalTokenNumber == TokenAsterisk ||
        CC71_GlobalTokenNumber == TokenPlus ||
        CC71_GlobalTokenNumber == TokenMinus ||
        CC71_GlobalTokenNumber == TokenBitwiseNot ||
        CC71_GlobalTokenNumber == TokenLogicalNot
    ) {
        CC71_GetToken();
        CC71_LogMessage(CC71_LOG_INFO, "Recognized a unary operator.");
        return TRUE;
    }
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////


/*int expression() {
    if (assignment_expression()) {
        return TRUE;
    } else if (expression()) {
        if (CC71_GlobalTokenNumber == TokenComma) {
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
    if (CC71_GlobalTokenNumber == TokenIf) {
        if (selection_statement()) {
            return TRUE;
        }
    } else if (CC71_GlobalTokenNumber == TokenOpenBrace) {
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
        if(CC71_GlobalTokenNumber == TokenSemicolon) {
                CC71_GetToken();
            return TRUE;
        }
    } else {
        if(CC71_GlobalTokenNumber == TokenSemicolon) {
            CC71_GetToken();
            return TRUE;
        }
    }
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////


int selection_statement() {
    if (CC71_GlobalTokenNumber == TokenIf) {
        CC71_GetToken();
        CC71_LogMessage(CC71_LOG_INFO, "Recognized 'if'.");

        if (CC71_GlobalTokenNumber == TokenOpenParentheses) {
            CC71_GetToken();
            CC71_LogMessage(CC71_LOG_INFO, "Recognized '('.");

            if (expression()) {
                CC71_LogMessage(CC71_LOG_INFO, "Recognized an expression.");

                if (CC71_GlobalTokenNumber == TokenCloseParentheses) {
                    CC71_GetToken();
                    CC71_LogMessage(CC71_LOG_INFO, "Recognized ')'.");

                    if(statement()) {
                        CC71_LogMessage(CC71_LOG_INFO, "Recognized a statement.");
                        return TRUE;
                    } else {
                        //AC71_ReportError(AC71_ERR_SYN_EXPECTED_CLOSE_PAREN, line, columnAux);
                        return FALSE;
                    }
                }
            }
        } else {
            //AC71_ReportError(AC71_ERR_SYN_EXPECTED_OPEN_PAREN, line, columnAux);
            return FALSE;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////


int compound_statement() {
    if (CC71_GlobalTokenNumber == TokenOpenBrace) {
        CC71_GetToken();
        CC71_LogMessage(CC71_LOG_INFO, "Recognized '{'.");

        statement();

        if (CC71_GlobalTokenNumber == TokenCloseBrace) {
            CC71_GetToken();
            CC71_LogMessage(CC71_LOG_INFO, "Recognized '}'.");
            return TRUE;
        }
    }
    return FALSE;
}










