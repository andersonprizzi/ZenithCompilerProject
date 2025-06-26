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

void marcaPosToken() {
	pilhacon[topcontexto].posglobal=ftell(inputFile);
	pilhacon[topcontexto].tkant = CC71_GlobalTokenNumber;
	pilhacon[topcontexto].cant=currentChar;
    strcpy(pilhacon[topcontexto].lexant,lex);
    topcontexto++;
}

/////////////////////////////////////////////////////////////////////////////

void restauraPosToken() {
    topcontexto--;
	fseek(inputFile,pilhacon[topcontexto].posglobal,SEEK_SET);
    currentChar=pilhacon[topcontexto].cant;
	CC71_GlobalTokenNumber = pilhacon[topcontexto].tkant;
    strcpy(lex,pilhacon[topcontexto].lexant);
}

/////////////////////////////////////////////////////////////////////////////

void CC71_GetNextChar() {
    fread(&currentChar, 1, 1, inputFile);

    if (feof(inputFile)) {
       currentChar =- 1;
       return;
    }

    if (currentChar == '\n') {
        line++;
        column = 0;
        columnAux = 0;
    } else {
        column++;
    }
}

/////////////////////////////////////////////////////////////////////////////

void CC71_GetToken() {
    int estado = 0;
    int fim = 0;
    int posl = 0;

    while (!fim) {
       lex[posl++] = currentChar;

       switch(estado){
            case 0:
                columnAux = column;
                // Reconhece e descarta comentários e reconhece divisão.
                if (currentChar == '/') {
                    CC71_GetNextChar();

                    // Reconhece e descarta coment�rios de uma linha.
                    if (currentChar == '/') {
                        posl--;
                        while (currentChar != '\n') CC71_GetNextChar();
                        printf("The lexical analyzer found a line comment at line %d.\n", line);
                        CC71_GetNextChar();
                        break;
                    }

                    // Reconhece e descarta coment�rios de múltiplas linhas.
                    else if (currentChar == '*') {
                        posl--;
                        int prev = 0;
                        int startingLine = line;
                        CC71_GetNextChar();

                        while (1) {
                            if (currentChar == -1) break; // EOF sem fechar coment�rio.
                            if (prev == '*' && currentChar == '/') {
                                printf("The lexical analyzer found a multi-line comment starting at line %d and ending at line %d.\n", startingLine, line);
                                CC71_GetNextChar();
                                break;
                            }
                            prev = currentChar;
                            CC71_GetNextChar();
                        }
                        break;
                    }

                    // Reconhece o operador de divis�o.
                    else {
                        lex[posl] = '\0';
                        CC71_GlobalTokenNumber = TokenDivision;
                        CC71_GetNextChar();
                        return;
                    }
                }

                // Reconhece identificadores.
                if ((currentChar >= 'a' && currentChar <= 'z') || (currentChar >= 'A' && currentChar <= 'Z') || (currentChar == '_')) {
                    CC71_GetNextChar();
                    estado = 1;
                    break;
                }

                // Reconhece números inteiros e em ponto flutuante.
                if ((currentChar >= '0') && (currentChar <= '9')) {
                    int isFloat = FALSE;
                    int hasDigitsAfterDot = FALSE;

                    // Parte inteira
                    while (currentChar >= '0' && currentChar <= '9') {
                        if (posl < MAX_TOKEN_LENGTH - 1)
                            lex[posl++] = currentChar;
                        CC71_GetNextChar();
                    }

                    // Parte decimal
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
                            CC71_ReportError(CC71_ERR_LEX_INVALID_NUMBER, line, column);
                            CC71_GlobalTokenNumber = -1;
                            return;
                        }
                    }

                    lex[posl] = '\0';
                    CC71_GlobalTokenNumber = isFloat ? TokenFloatConst : TokenIntConst;
                    return;
                }

                // Reconhece uma cadeia de caracteres.
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

                        // Proteção contra overflow
                        if (posl >= MAX_TOKEN_LENGTH - 1) break;

                        CC71_GetNextChar();
                    }
                    CC71_GetNextChar(); // consume closing "
                    lex[posl] = '\0';
                    CC71_GlobalTokenNumber = TokenString;
                    return;
                }

                // Reconhece o operador de igualdade e atribui��o.
                if (currentChar == '=') {
                    CC71_GetNextChar();

                    // Reconhece o operador de igualdade.
                    if (currentChar == '=') {
                       lex[posl++] = '=';
                       lex[posl]='\0';
                       CC71_GetNextChar();
                       CC71_GlobalTokenNumber = TokenEqual;
                       return;
                    }

                    // Reconhece o operador de atribui��o.
                    else {
                       lex[posl] = '\0';
                       CC71_GlobalTokenNumber = TokenAssign;
                       return;
                    }
                }

                // Reconhece os operadores de incremento, atribui��o por adi��o e adi��o.
                 if (currentChar == '+') {
                    CC71_GetNextChar();

                    // Reconhece o operador de incremento.
                    if (currentChar == '+') {
                       lex[posl++]='+';
                       lex[posl]='\0';
                       CC71_GetNextChar();
                       CC71_GlobalTokenNumber = TokenIncrement;
                       return;
                    }

                    // Reconhece o operador de atriui��o por adi��o.
                    else if (currentChar == '=') {
                       lex[posl++] = '=';
                       lex[posl] = '\0';
                       CC71_GetNextChar();
                       CC71_GlobalTokenNumber = TokenPlusAssign;
                       return;
                    }

                    // Reconhece o operador de adic�o.
                    else {
                       lex[posl] = '\0';
                       CC71_GlobalTokenNumber = TokenPlus;
                       return;
                    }
                }

                // Reconhece os operadores de decremento e subtra��o.
                if (currentChar == '-') {
                    CC71_GetNextChar();

                    // Reconhece o operador de decremento.
                    if (currentChar == '-') {
                        lex[posl++] = '-';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenDecrement;
                        return;
                    }

                    // Reconhece o operador de acesso de ponteiro (seta).
                    else if (currentChar == '>') {
                        lex[posl++] = '>';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenArrow;
                        return;
                    }

                    // Reconhece o operador de subtra��o.
                    else {
                       lex[posl] = '\0';
                       CC71_GlobalTokenNumber = TokenMinus;
                       return;
                    }
                }

                // Reconhece o operador de multiplica��o.
                if (currentChar == '*') {
                    lex[posl]='\0';
                    CC71_GetNextChar();
                    CC71_GlobalTokenNumber = TokenAsterisk;
                    return;
                }

                // Reconhece o operador de m�dulo e %>.
                if (currentChar == '%') {
                    CC71_GetNextChar();

                    // Reconhece o operador %>.
                    if (currentChar == '>') {
                        lex[posl++] = '>';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenCloseBrace;
                        return;
                    }

                    // Reconhece o operador de m�dulo.
                    else {
                        lex[posl]='\0';
                        CC71_GlobalTokenNumber = TokenMod;
                        return;
                    }
                }

                // Reconhece o operador de acesso de membro de estrutura.
                if (currentChar == '.') {
                    lex[posl]='\0';
                    CC71_GetNextChar();
                    CC71_GlobalTokenNumber = TokenDot;
                    return;
                }

                // Reconhece o operador de interroga��o do conjunto tern�rio.
                if (currentChar == '?') {
                    lex[posl]='\0';
                    CC71_GetNextChar();
                    CC71_GlobalTokenNumber = TokenQuestionMark;
                    return;
                }

                // Reconhece o operador lógico 'and', operador bitwise 'and' e o operador 'address of'.
                if (currentChar == '&') {
                    CC71_GetNextChar();
                    
                    // Reconhece o operador lógico 'and'.
                    if (currentChar == '&') {
                        lex[posl++] = '&';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenLogicalAnd;
                        return;
                    }

                    // Reconhece o operador &=.
                    else if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenAndAssign;
                        return;
                    }

                    // Reconhece o operador bitwise 'and' e o operador 'address of'.
                    else {
                        lex[posl] = '\0';
                        CC71_GlobalTokenNumber = TokenBitwiseAnd_AddressOf;
                        return;
                    }
                }

                // Reconhece o operador lógico 'or' e o operador bitwise 'or'.
                if (currentChar == '|') {
                    CC71_GetNextChar();

                    // Reconhece o operador lógico 'or'.
                    if (currentChar == '|') {
                       lex[posl++] = '|';
                       lex[posl] = '\0';
                       CC71_GetNextChar();
                       CC71_GlobalTokenNumber = TokenLogicalOr;
                       return;
                    }

                    // Reconheceu o operador |=.
                    else if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenOrAssign;
                        return;
                    }

                    // Reconhece o operador bitwise 'or'.
                    else {
                        lex[posl]='\0';
                        CC71_GlobalTokenNumber = TokenBitwiseOr;
                        return;
                    }
                }

                // Reconhece o operador bitwise 'xor'.
                if (currentChar == '^') {
                    CC71_GetNextChar();

                    if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenXorAssign;
                        return;
                    }
                    else {
                        lex[posl] = '\0';
                        CC71_GlobalTokenNumber = TokenBitwiseXor;
                        return;
                    }
                }

                // Reconhece o operador bitwise 'not'.
                if (currentChar == '~') {
                    lex[posl] = '\0';
                    CC71_GetNextChar();
                    CC71_GlobalTokenNumber = TokenBitwiseNot;
                    return;
                }

                // Reconhece o operador de deslocamento � direita, maior ou igual e maior que.
                if (currentChar == '>') {
                    CC71_GetNextChar();

                    if (currentChar == '>') {
                        CC71_GetNextChar();

                        if (currentChar == '=') {
                            lex[posl++] = '>';
                            lex[posl++] = '=';
                            lex[posl] = '\0';
                            CC71_GetNextChar();
                            CC71_GlobalTokenNumber = TokenRightShiftAssign;
                            return;
                        }
                        else {
                            lex[posl++] = '>';
                            lex[posl] = '\0';
                            CC71_GlobalTokenNumber = TokenRightShift;
                            return;
                        }
                    }
                    else if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenGreaterEqual;
                        return;
                    }
                    else {
                    lex[posl] = '\0';
                    CC71_GlobalTokenNumber = TokenGreaterThan;
                    return;
                    }
                }

                // Reconhece o operador de deslocamento � esquerda, menor ou igual, menor que e <%.
                if (currentChar == '<') {
                    CC71_GetNextChar();

                    if (currentChar == '<') {
                        CC71_GetNextChar();

                        if (currentChar == '=') {
                            lex[posl++] = '<';
                            lex[posl++] = '=';
                            lex[posl] = '\0';
                            CC71_GetNextChar();
                            CC71_GlobalTokenNumber = TokenLeftShiftAssign;
                            return;
                        }
                        else {
                            lex[posl++] = '<';
                            lex[posl] = '\0';
                            CC71_GlobalTokenNumber = TokenLeftShift;
                            return;
                        }
                    }
                    else if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenLessEqual;
                        return;
                    }
                    else if (currentChar == '%') {
                        lex[posl++] = '%';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenOpenBrace;
                        return;
                    }
                    else if (currentChar == ':') {
                        lex[posl++] = ':';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenOpenBracket;
                        return;
                    }
                    else {
                    lex[posl] = '\0';
                    CC71_GlobalTokenNumber = TokenLessThan;
                    return;
                    }
                }

                // Reconhece o operador de diferen�a e nega��o l�gica.
                if (currentChar == '!') {
                    CC71_GetNextChar();

                    // Reconhece o operador de diferen�a.
                    if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenNotEqual;
                        return;
                    }

                    // Reconhece o operador de nega��o l�gica.
                    else {
                       lex[posl] = '\0';
                       CC71_GlobalTokenNumber = TokenLogicalNot;
                       return;
                    }
                }

                // Reconhece os operadores dois pontos e :>.
                if (currentChar == ':') {
                    CC71_GetNextChar();

                    // Reconhece o operador :>.
                    if (currentChar == '>') {
                        lex[posl++] = '>';
                        lex[posl] = '\0';
                        CC71_GetNextChar();
                        CC71_GlobalTokenNumber = TokenCloseBracket;
                        return;
                    }

                    // Reconhece o operador dois pontos.
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

                printf("ERROR: The lexical analyzer found the character %c (%d) on line %d.\n",currentChar,currentChar,line);
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
