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
// LEXER                                                                   //
/////////////////////////////////////////////////////////////////////////////

int palavra_reservada(char lex[]) {
    int postab = 0;
    while (strcmp("identifier", keywordList[postab].tokenWord) != 0) {
       if (strcmp(lex, keywordList[postab].tokenWord) == 0)
          return keywordList[postab].tokenNumber;
       postab++;
    }
    return TokenIdentifier;
}

/////////////////////////////////////////////////////////////////////////////

void CC71_BacktrackingStart() {
    long pos = ftell(CC71_GlobalInputFile);
    pilhacon[topcontexto].posglobal = pos;
    pilhacon[topcontexto].tkant = CC71_GlobalTokenNumber;
    pilhacon[topcontexto].cant = currentChar;
    strcpy(pilhacon[topcontexto].lexant, lex);
    topcontexto++;
    //CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "[EMPILHA] Current token = %d ('%s'); context index = %d;", CC71_GlobalTokenNumber, lex, topcontexto);
}

/////////////////////////////////////////////////////////////////////////////

void CC71_BacktrackingRestore() {
    if (topcontexto == 0) {
        //CC71_LogMessage(CC71_LOG_ERROR, "restauraPosToken: tentativa de restauracaoo com pilha vazia (topcontexto = 0)");
        return;
    }

    topcontexto--;
    long pos = pilhacon[topcontexto].posglobal;
    fseek(CC71_GlobalInputFile, pos, SEEK_SET);
    currentChar = pilhacon[topcontexto].cant;
    CC71_GlobalTokenNumber = pilhacon[topcontexto].tkant;
    strcpy(lex, pilhacon[topcontexto].lexant);
    //CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "[DESEMPILHA] token restaurado = %d ('%s'), indice de contexto = %d", CC71_GlobalTokenNumber, lex, topcontexto);
}

/////////////////////////////////////////////////////////////////////////////

void CC71_BacktrackingEnd() {
    if (topcontexto > 0) {
        topcontexto--;
        //CC71_LogMessage(CC71_LOG_DEBUG, CC71_LOG_EVENT_GENERIC, "[DESEMPILHA] Desempilhando contexto. Token atual = %d ('%s'), novo indice de contexto = %d", CC71_GlobalTokenNumber, lex, topcontexto);
    }
}

/////////////////////////////////////////////////////////////////////////////

void CC71_GetNextChar() {
    fread(&currentChar, 1, 1, CC71_GlobalInputFile);

    if (feof(CC71_GlobalInputFile)) {
       currentChar =- 1;
       return;
    }

    if (currentChar == '\n') {
        CC71_GlobalCurrentLine++;
        CC71_GlobalCurrentColumn = 0;
        columnAux = 0;
    } else {
        CC71_GlobalCurrentColumn++;
    }
}

/////////////////////////////////////////////////////////////////////////////

void CC71_GetToken() {
    int estado = 0;
    int posl = 0;

    while (1) {
       lex[posl++] = currentChar;

       switch(estado){
            case 0:
                columnAux = CC71_GlobalCurrentColumn;
                // Recognizes and skips comments or identifies the division operator.
                if (currentChar == '/') {
                    CC71_GetNextChar();

                    // Recognizes and skips single-line comments.
                    if (currentChar == '/') {
                        posl--;
                        while (currentChar != '\n') CC71_GetNextChar();
                        CC71_GetNextChar();
                        break;
                    }

                    // Recognizes the division assignment operator (-=).
                    else if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenDivAssign;
                        return;
                    }

                    // Recognizes and skips multi-line comments.
                    else if (currentChar == '*') {
                        posl--;
                        int prev = 0;
                        int startingLine = CC71_GlobalCurrentLine;
                        CC71_GetNextChar();

                        while (1) {
                            if (currentChar == -1) break;
                            if (prev == '*' && currentChar == '/') {
                                CC71_GetNextChar();
                                break;
                            }
                            prev = currentChar;
                            CC71_GetNextChar();
                        }
                        break;
                    }

                    // Recognizes the division operator.
                    else {
                        lex[posl] = '\0';
                        CC71_GlobalTokenNumber = TokenDivision;
                        CC71_GetNextChar();
                        return;
                    }
                }

                // Recognizes identifiers.
                if ((currentChar >= 'a' && currentChar <= 'z') || (currentChar >= 'A' && currentChar <= 'Z') || (currentChar == '_')) {
                    CC71_GetNextChar();
                    estado = 1;
                    break;
                }

                // Recognizes numbers.
                if ((currentChar >= '0') && (currentChar <= '9')) {
                    int isFloat = FALSE;
                    int hasDigitsAfterDot = FALSE;

                    // Parses the integer part of the numeric literal.
                    while (currentChar >= '0' && currentChar <= '9') {
                        if (posl < MAX_TOKEN_LENGTH - 1)
                            lex[posl++] = currentChar;
                        CC71_GetNextChar();
                    }

                    // Parses the decimal part of the floating-point literal.
                    if (currentChar == '.') {
                        isFloat = TRUE;
                        if (posl < MAX_TOKEN_LENGTH - 1) lex[posl++] = currentChar;
                        CC71_GetNextChar();

                        while (currentChar >= '0' && currentChar <= '9') {
                            hasDigitsAfterDot = TRUE;
                            if (posl < MAX_TOKEN_LENGTH - 1) lex[posl++] = currentChar;
                            CC71_GetNextChar();
                        }

                        if (!hasDigitsAfterDot) {
                            CC71_ReportError(CC71_ERR_LEX_INVALID_CHAR, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, "Unexpected character '%c' (ASCII %d).", currentChar, currentChar);
                            CC71_GlobalTokenNumber = -1;
                            return;
                        }
                    }

                    lex[posl] = '\0';
                    CC71_GlobalTokenNumber = isFloat ? TokenFloatConst : TokenIntConst;
                    return;
                }

                // Recognizes character constants.
                if (currentChar == '\'') {
                    CC71_GetNextChar();
                    int charValue = 0;

                    if (currentChar == '\\') {
                        CC71_GetNextChar();
                        
                        switch (currentChar) {
                            case 'n': charValue = '\n'; break;
                            case 't': charValue = '\t'; break;
                            case 'r': charValue = '\r'; break;
                            case '0': charValue = '\0'; break;
                            case '\\': charValue = '\\'; break;
                            case '\'': charValue = '\''; break;
                            case '\"': charValue = '\"'; break;
                            case 'x': {
                                char hexbuf[5] = {0};
                                int i = 0;
                                CC71_GetNextChar();
                                while (isxdigit(currentChar) && i < 4) {
                                    hexbuf[i++] = currentChar;
                                    CC71_GetNextChar();
                                }
                                charValue = (char)strtol(hexbuf, NULL, 16);
                                break;
                            }

                            default:
                                CC71_ReportError(CC71_ERR_LEX_INVALID_CHAR, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, "Escape inválido: \\%c", currentChar);
                                CC71_GlobalTokenNumber = -1;
                                return;
                        }

                    } else {
                        charValue = currentChar;
                        CC71_GetNextChar();
                    }

                    if (currentChar != '\'') {
                        CC71_ReportError(CC71_ERR_LEX_INVALID_CHAR, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, "Esperado fechamento de caractere: '");
                        CC71_GlobalTokenNumber = -1;
                        return;
                    }

                    lex[0] = charValue;
                    lex[1] = '\0';

                    CC71_GetNextChar();

                    CC71_GlobalTokenNumber = TokenCharConst;
                    return;
                }

                // Recognizes a string literal.
                if (currentChar == '"') {
                    CC71_GetNextChar();
                    while (currentChar != '"' && currentChar != -1) {
                        if (currentChar == '\\') {
                            CC71_GetNextChar();
                            if (currentChar == 'n') {
                                lex[posl++] = '\n';
                            } else if (currentChar == 't') {
                                lex[posl++] = '\t';
                            } else if (currentChar == 'r') {
                                lex[posl++] = '\r';
                            } else if (currentChar == '\\') {
                                lex[posl++] = '\\';
                            } else if (currentChar == '"') {
                                lex[posl++] = '"';
                            } else {
                                lex[posl++] = '\\';
                                lex[posl++] = currentChar;
                            }
                        } else {
                            lex[posl++] = currentChar;
                        }
                        
                        if (posl >= MAX_TOKEN_LENGTH - 1) break;
                        CC71_GetNextChar();
                    }
                    CC71_GetNextChar();
                    lex[posl] = '\0';
                    CC71_GlobalTokenNumber = TokenString;
                    return;
                }

                // 
                if (currentChar == '=') {
                    CC71_GetNextChar();

                    // Recognizes the equality operator (==).
                    if (currentChar == '=') {
                       lex[posl++] = '=';
                       lex[posl]='\0';
                       CC71_GetNextChar();
                       CC71_GlobalTokenNumber = TokenEqual;
                       return;
                    }

                    // Recognizes the assignment operator (=).
                    else {
                       lex[posl] = '\0';
                       CC71_GlobalTokenNumber = TokenAssign;
                       return;
                    }
                }

                // 
                 if (currentChar == '+') {
                    CC71_GetNextChar();

                    // Recognizes the increment operator (++).
                    if (currentChar == '+') {
                       lex[posl++]='+';
                       lex[posl]='\0';
                       CC71_GetNextChar();
                       CC71_GlobalTokenNumber = TokenIncrement;
                       return;
                    }

                    // Recognizes the addition assignment operator (+=).
                    else if (currentChar == '=') {
                       lex[posl++] = '=';
                       lex[posl] = '\0';
                       CC71_GetNextChar();
                       CC71_GlobalTokenNumber = TokenPlusAssign;
                       return;
                    }

                    // Recognizes the addition operator (+).
                    else {
                       lex[posl] = '\0';
                       CC71_GlobalTokenNumber = TokenPlus;
                       return;
                    }
                }

                // 
                if (currentChar == '-') {
                    CC71_GetNextChar();

                    // Recognizes the decrement operator (--).
                    if (currentChar == '-') {
                        lex[posl++] = '-';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenDecrement;
                        return;
                    }

                    // Recognizes the pointer access operator (arrow '->').
                    else if (currentChar == '>') {
                        lex[posl++] = '>';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenArrow;
                        return;
                    }

                    // Recognizes the subtraction assignment operator (-=).
                    else if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenMinusAssign;
                        return;
                    }

                    // Recognizes the subtraction operator (-).
                    else {
                       lex[posl] = '\0';
                       CC71_GlobalTokenNumber = TokenMinus;
                       return;
                    }
                }

                // Recognizes the multiplication operator (*) and multiplication assignment operator (*=).
                if (currentChar == '*') {
                    CC71_GetNextChar();

                    if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenMulAssign;
                        return;
                    } else {
                        lex[posl] = '\0';
                        CC71_GlobalTokenNumber = TokenAsterisk;
                        return;
                    }
                }

                // Recognizes the modulo operator (%) and compound token %> (close brace).
                if (currentChar == '%') {
                    CC71_GetNextChar();

                    // Recognizes the compound token %> (close brace).
                    if (currentChar == '>') {
                        lex[posl++] = '>';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenCloseBrace;
                        return;
                    }

                    // Recognizes the modulo assignment operator (%=).
                    else if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenModAssign;
                        return;
                    }

                    // Recognizes the modulo operator (%).
                    else {
                        lex[posl]='\0';
                        CC71_GlobalTokenNumber = TokenMod;
                        return;
                    }
                }

                // Recognizes the structure member access operator (dot '.') and the ellipsis operator (...).
                if (currentChar == '.') {
                    CC71_GetNextChar();

                    if (currentChar == '.') {
                        CC71_GetNextChar();

                        if (currentChar == '.') {
                            lex[posl++] = '.';
                            lex[posl++] = '.';
                            lex[posl] = '\0';
                            CC71_GetNextChar();
                            CC71_GlobalTokenNumber = TokenEllipsis;
                            return;
                        } else {
                            CC71_ReportError(CC71_ERR_LEX_INVALID_CHAR, CC71_GlobalCurrentLine, CC71_GlobalCurrentColumn, "Unexpected character '%c' after '..' (expected another '.').", currentChar);
                            CC71_GlobalTokenNumber = -1;
                            return;
                        }
                    }
                    
                    lex[posl] = '\0';
                    CC71_GlobalTokenNumber = TokenDot;
                    return;
                }

                // Recognizes the ternary conditional operator '?'.
                if (currentChar == '?') {
                    lex[posl]='\0';
                    CC71_GetNextChar();
                    CC71_GlobalTokenNumber = TokenQuestionMark;
                    return;
                }

                // 
                if (currentChar == '&') {
                    CC71_GetNextChar();
                    
                    // Recognizes the logical AND operator (&&).
                    if (currentChar == '&') {
                        lex[posl++] = '&';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenLogicalAnd;
                        return;
                    }

                    // Recognizes the bitwise AND assignment operator (&=)
                    else if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenAndAssign;
                        return;
                    }

                    // Recognizes the bitwise AND operator (&) and the address-of operator (&).
                    else {
                        lex[posl] = '\0';
                        CC71_GlobalTokenNumber = TokenBitwiseAnd_AddressOf;
                        return;
                    }
                }

                // 
                if (currentChar == '|') {
                    CC71_GetNextChar();

                    // Recognizes the logical OR operator (||).
                    if (currentChar == '|') {
                       lex[posl++] = '|';
                       lex[posl] = '\0';
                       CC71_GetNextChar();
                       CC71_GlobalTokenNumber = TokenLogicalOr;
                       return;
                    }

                    // Recognizes the bitwise OR assignment operator (|=).
                    else if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenOrAssign;
                        return;
                    }

                    // Recognizes the bitwise OR operator (|).
                    else {
                        lex[posl] = '\0';
                        CC71_GlobalTokenNumber = TokenBitwiseOr;
                        return;
                    }
                }

                // Recognizes the bitwise XOR operator (^) and the XOR assignment operator (^=).
                if (currentChar == '^') {
                    CC71_GetNextChar();

                    // Recognizes the XOR assignment operator (^=).
                    if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenXorAssign;
                        return;
                    }

                    // // Recognizes the bitwise XOR operator (^).
                    else {
                        lex[posl] = '\0';
                        CC71_GlobalTokenNumber = TokenBitwiseXor;
                        return;
                    }
                }

                // Recognizes the bitwise NOT operator (~).
                if (currentChar == '~') {
                    lex[posl] = '\0';
                    CC71_GetNextChar();
                    CC71_GlobalTokenNumber = TokenBitwiseNot;
                    return;
                }

                // 
                if (currentChar == '>') {
                    CC71_GetNextChar();
                    
                    if (currentChar == '>') {
                        CC71_GetNextChar();

                        // Recognizes the right shift assignment operator (>>=).
                        if (currentChar == '=') {
                            lex[posl++] = '>';
                            lex[posl++] = '=';
                            lex[posl] = '\0';
                            CC71_GetNextChar();
                            CC71_GlobalTokenNumber = TokenRightShiftAssign;
                            return;
                        }

                        // Recognizes the right shift operator (>>).
                        else {
                            lex[posl++] = '>';
                            lex[posl] = '\0';
                            CC71_GlobalTokenNumber = TokenRightShift;
                            return;
                        }
                    }

                    // Recognizes the greater than or equal operator (>=).
                    else if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenGreaterEqual;
                        return;
                    }

                    // Recognizes the greater than operator (>).
                    else {
                        lex[posl] = '\0';
                        CC71_GlobalTokenNumber = TokenGreaterThan;
                        return;
                    }
                }

                // 
                if (currentChar == '<') {
                    CC71_GetNextChar();

                    if (currentChar == '<') {
                        CC71_GetNextChar();

                         // Recognizes the left shift assignment operator (<<=).
                        if (currentChar == '=') {
                            lex[posl++] = '<';
                            lex[posl++] = '=';
                            lex[posl] = '\0';
                            CC71_GetNextChar();
                            CC71_GlobalTokenNumber = TokenLeftShiftAssign;
                            return;
                        }

                        // Recognizes the left shift operator (<<).
                        else {
                            lex[posl++] = '<';
                            lex[posl] = '\0';
                            CC71_GlobalTokenNumber = TokenLeftShift;
                            return;
                        }
                    }

                    // Recognizes the less than or equal operator (<=).
                    else if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenLessEqual;
                        return;
                    }

                    // Recognizes the compound token <% (open brace).
                    else if (currentChar == '%') {
                        lex[posl++] = '%';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenOpenBrace;
                        return;
                    }

                     // Recognizes the compound token <: (open bracket).
                    else if (currentChar == ':') {
                        lex[posl++] = ':';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenOpenBracket;
                        return;
                    }

                    // Recognizes the less than operator (<).
                    else {
                        lex[posl] = '\0';
                        CC71_GlobalTokenNumber = TokenLessThan;
                        return;
                    }
                }

                // 
                if (currentChar == '!') {
                    CC71_GetNextChar();

                    // Recognizes the inequality operator (!=).
                    if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenNotEqual;
                        return;
                    }

                    // Recognizes the logical NOT operator (!).
                    else {
                       lex[posl] = '\0';
                       CC71_GlobalTokenNumber = TokenLogicalNot;
                       return;
                    }
                }

                // 
                if (currentChar == ':') {
                    CC71_GetNextChar();

                    // Recognizes the compound token :> (close bracket).
                    if (currentChar == '>') {
                        lex[posl++] = '>';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenCloseBracket;
                        return;
                    }

                    // Recognizes the colon operator (:).
                    else {
                       lex[posl] = '\0';
                       CC71_GlobalTokenNumber = TokenColon;
                       return;
                    }
                }

                if (currentChar=='['){lex[posl]='\0'; CC71_GetNextChar(); CC71_GlobalTokenNumber=TokenOpenBracket; return;}
                if (currentChar==']'){lex[posl]='\0'; CC71_GetNextChar(); CC71_GlobalTokenNumber=TokenCloseBracket; return;}
                if (currentChar=='('){lex[posl]='\0'; CC71_GetNextChar(); CC71_GlobalTokenNumber=TokenOpenParentheses; return;}
                if (currentChar==')'){lex[posl]='\0'; CC71_GetNextChar(); CC71_GlobalTokenNumber=TokenCloseParentheses; return;}
                if (currentChar=='{'){lex[posl]='\0'; CC71_GetNextChar(); CC71_GlobalTokenNumber=TokenOpenBrace; return;}
                if (currentChar=='}'){lex[posl]='\0'; CC71_GetNextChar(); CC71_GlobalTokenNumber=TokenCloseBrace; return;}
                if (currentChar==','){lex[posl]='\0'; CC71_GetNextChar(); CC71_GlobalTokenNumber=TokenComma; return;}
                if (currentChar==';'){lex[posl]='\0'; CC71_GetNextChar(); CC71_GlobalTokenNumber=TokenSemicolon; return;}
                if (currentChar==-1) {lex[posl]='\0'; CC71_GetNextChar(); CC71_GlobalTokenNumber=TokenEndOfFile; return;}
                if (currentChar==' ' || currentChar=='\n' || currentChar=='\t' || currentChar=='\r') {CC71_GetNextChar();posl--;break;}
                if (currentChar=='\0') {CC71_GlobalTokenNumber=-1;return;}

                //printf("ERROR: The lexical analyzer found the character %c (%d) on line %d.\n",currentChar,currentChar,line);
                while (currentChar != '\n') CC71_GetNextChar();
                break;

            /////////////////////////////////////////////////////////////////

            case 1:
                if ((currentChar>='a' && currentChar<='z') || (currentChar>='A' && currentChar<='Z') || (currentChar == '_') || (currentChar>='0' && currentChar<='9')) {
                    CC71_GetNextChar();
                    break;
                }

                lex[--posl] = '\0';
                CC71_GlobalTokenNumber = palavra_reservada(lex);
                return;
          }
       }
}
