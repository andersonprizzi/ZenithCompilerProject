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
	pilhacon[topcontexto].tkant = tokenNumber;
	pilhacon[topcontexto].cant=currentChar;
    strcpy(pilhacon[topcontexto].lexant,lex);
    topcontexto++;
}

/////////////////////////////////////////////////////////////////////////////

void restauraPosToken() {
    topcontexto--;
	fseek(inputFile,pilhacon[topcontexto].posglobal,SEEK_SET);
    currentChar=pilhacon[topcontexto].cant;
	tokenNumber = pilhacon[topcontexto].tkant;
    strcpy(lex,pilhacon[topcontexto].lexant);
}

/////////////////////////////////////////////////////////////////////////////

void getNextChar() {
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

void getToken() {
    int estado = 0;
    int fim = 0;
    int posl = 0;

    while (!fim) {
       lex[posl++] = currentChar;

       switch(estado){
            case 0:
                columnAux = column;
                // Reconhece e descarta coment�rios e reconhece divis�o.
                if (currentChar == '/') {
                    getNextChar();

                    // Reconhece e descarta coment�rios de uma linha.
                    if (currentChar == '/') {
                        posl--;
                        while (currentChar != '\n') getNextChar();
                        printf("The lexical analyzer found a line comment at line %d.\n", line);
                        getNextChar();
                        break;
                    }

                    // Reconhece e descarta coment�rios de m�ltiplas linhas.
                    else if (currentChar == '*') {
                        posl--;
                        int prev = 0;
                        int startingLine = line;
                        getNextChar();

                        while (1) {
                            if (currentChar == -1) break; // EOF sem fechar coment�rio.
                            if (prev == '*' && currentChar == '/') {
                                printf("The lexical analyzer found a multi-line comment starting at line %d and ending at line %d.\n", startingLine, line);
                                getNextChar();
                                break;
                            }
                            prev = currentChar;
                            getNextChar();
                        }
                        break;
                    }

                    // Reconhece o operador de divis�o.
                    else {
                        lex[posl] = '\0';
                        tokenNumber = TokenDivision;
                        getNextChar();
                        return;
                    }
                }

                // Reconhece identificadores.
                if ((currentChar >= 'a' && currentChar <= 'z') || (currentChar >= 'A' && currentChar <= 'Z') || (currentChar == '_')) {
                    getNextChar();
                    estado = 1;
                    break;
                }

                // Reconhece n�meros inteiros e em ponto flutuante.
                if ((currentChar >= '0') && (currentChar <= '9')) {
                    int floatConstFlag = FALSE;
                    while ((currentChar >= '0') && (currentChar <= '9') || (currentChar == '.')) {
                        getNextChar();
                        if((currentChar >= '0' && currentChar <= '9')) {
                            lex[posl++] = currentChar;
                        } else if (currentChar == '.') {
                            lex[posl++] = currentChar;
                            floatConstFlag = TRUE;
                        }
                    }

                    lex[posl] = '\0';
                    if (floatConstFlag) {
                        tokenNumber = TokenFloatConst;
                    } else {
                        tokenNumber = TokenIntConst;
                    }
                    return;
                }

                // Reconhece uma cadeia de caracteres.
                if (currentChar == '"') {
                    getNextChar();
                    while (currentChar!='"') {
                       lex[posl++]=currentChar;
                       getNextChar();
                    }
                    getNextChar();
                    lex[posl]='\0';
                    tokenNumber = TokenString;
                    return;
                }

                // Reconhece o operador de igualdade e atribui��o.
                if (currentChar == '=') {
                    getNextChar();

                    // Reconhece o operador de igualdade.
                    if (currentChar == '=') {
                       lex[posl++] = '=';
                       lex[posl]='\0';
                       getNextChar();
                       tokenNumber = TokenEqual;
                       return;
                    }

                    // Reconhece o operador de atribui��o.
                    else {
                       lex[posl] = '\0';
                       tokenNumber = TokenAssign;
                       return;
                    }
                }

                // Reconhece os operadores de incremento, atribui��o por adi��o e adi��o.
                 if (currentChar == '+') {
                    getNextChar();

                    // Reconhece o operador de incremento.
                    if (currentChar == '+') {
                       lex[posl++]='+';
                       lex[posl]='\0';
                       getNextChar();
                       tokenNumber = TokenIncrement;
                       return;
                    }

                    // Reconhece o operador de atriui��o por adi��o.
                    else if (currentChar == '=') {
                       lex[posl++] = '=';
                       lex[posl] = '\0';
                       getNextChar();
                       tokenNumber = TokenPlusAssign;
                       return;
                    }

                    // Reconhece o operador de adic�o.
                    else {
                       lex[posl] = '\0';
                       tokenNumber = TokenPlus;
                       return;
                    }
                }

                // Reconhece os operadores de decremento e subtra��o.
                if (currentChar == '-') {
                    getNextChar();

                    // Reconhece o operador de decremento.
                    if (currentChar == '-') {
                        lex[posl++] = '-';
                        lex[posl] = '\0';
                        getNextChar();
                        tokenNumber = TokenDecrement;
                        return;
                    }

                    // Reconhece o operador de acesso de ponteiro (seta).
                    else if (currentChar == '>') {
                        lex[posl++] = '>';
                        lex[posl] = '\0';
                        getNextChar();
                        tokenNumber = TokenArrow;
                        return;
                    }

                    // Reconhece o operador de subtra��o.
                    else {
                       lex[posl] = '\0';
                       tokenNumber = TokenMinus;
                       return;
                    }
                }

                // Reconhece o operador de multiplica��o.
                if (currentChar == '*') {
                    lex[posl]='\0';
                    getNextChar();
                    tokenNumber = TokenAsterisk;
                    return;
                }

                // Reconhece o operador de m�dulo e %>.
                if (currentChar == '%') {
                    getNextChar();

                    // Reconhece o operador %>.
                    if (currentChar == '>') {
                        lex[posl++] = '>';
                        lex[posl] = '\0';
                        getNextChar();
                        tokenNumber = TokenCloseBrace;
                        return;
                    }

                    // Reconhece o operador de m�dulo.
                    else {
                        lex[posl]='\0';
                        tokenNumber = TokenMod;
                        return;
                    }
                }

                // Reconhece o operador de acesso de membro de estrutura.
                if (currentChar == '.') {
                    lex[posl]='\0';
                    getNextChar();
                    tokenNumber = TokenDot;
                    return;
                }

                // Reconhece o operador de interroga��o do conjunto tern�rio.
                if (currentChar == '?') {
                    lex[posl]='\0';
                    getNextChar();
                    tokenNumber = TokenQuestionMark;
                    return;
                }

                // Reconhece o operador l�gico 'and', operador bitwise 'and' e o operador 'address of'.
                if (currentChar == '&') {
                    getNextChar();

                    // Reconhece o operador l�gico 'and'.
                    if (currentChar == '&') {
                       lex[posl++] = '&';
                       lex[posl] = '\0';
                       getNextChar();
                       tokenNumber = TokenLogicalAnd;
                       return;
                    }

                    // Reconhece o operador bitwise 'and' e o operador 'address of'.
                    else {
                        lex[posl]='\0';
                        tokenNumber = TokenBitwiseAnd_AddressOf;
                        return;
                    }
                }

                // Reconhece o operador l�gico 'or' e o operador bitwise 'or'.
                if (currentChar == '|') {
                    getNextChar();

                    // Reconhece o operador l�gico 'or'.
                    if (currentChar == '|') {
                       lex[posl++] = '|';
                       lex[posl] = '\0';
                       getNextChar();
                       tokenNumber = TokenLogicalOr;
                       return;
                    }

                    // Reconhece o operador bitwise 'or'.
                    else {
                        lex[posl]='\0';
                        tokenNumber = TokenBitwiseOr;
                        return;
                    }
                }

                // Reconhece o operador bitwise 'xor'.
                if (currentChar == '^') {
                    lex[posl] = '\0';
                    getNextChar();
                    tokenNumber = TokenBitwiseXor;
                    return;
                }

                // Reconhece o operador bitwise 'not'.
                if (currentChar == '~') {
                    lex[posl] = '\0';
                    getNextChar();
                    tokenNumber = TokenBitwiseNot;
                    return;
                }

                // Reconhece o operador de deslocamento � direita, maior ou igual e maior que.
                if (currentChar == '>') {
                    getNextChar();

                    // Reconhece o operador bitwise de deslocamento � direita.
                    if (currentChar == '>') {
                        lex[posl++] = '>';
                        lex[posl] = '\0';
                        getNextChar();
                        tokenNumber = TokenRightShift;
                        return;
                    }

                    // Reconhece o operador de compara��o maior ou igual.
                    else if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        getNextChar();
                        tokenNumber = TokenGreaterEqual;
                        return;
                    }

                    // Reconhece o operador de compara��o maior.
                    else {
                       lex[posl] = '\0';
                       tokenNumber = TokenGreaterThan;
                       return;
                    }
                }

                // Reconhece o operador de deslocamento � esquerda, menor ou igual, menor que e <%.
                if (currentChar == '<') {
                    getNextChar();

                    // Reconhece o operador bitwise de deslocamento � esquerda.
                    if (currentChar == '<') {
                        lex[posl++] = '<';
                        lex[posl] = '\0';
                        getNextChar();
                        tokenNumber = TokenLeftShift;
                        return;
                    }

                    // Reconhece o operador de compara��o menor ou igual.
                    else if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        getNextChar();
                        tokenNumber = TokenLessEqual;
                        return;
                    }

                    // Reconhece o operador <%.
                    else if (currentChar == '%') {
                        lex[posl++] = '%';
                        lex[posl] = '\0';
                        getNextChar();
                        tokenNumber = TokenOpenBrace;
                        return;
                    }

                    // Reconhece o operador <:.
                    if (currentChar == ':') {
                        lex[posl++] = ':';
                        lex[posl] = '\0';
                        getNextChar();
                        tokenNumber = TokenOpenBracket;
                        return;
                    }

                    // Reconhece o operador de compara��o menor.
                    else {
                       lex[posl] = '\0';
                       tokenNumber = TokenLessThan;
                       return;
                    }
                }

                // Reconhece o operador de diferen�a e nega��o l�gica.
                if (currentChar == '!') {
                    getNextChar();

                    // Reconhece o operador de diferen�a.
                    if (currentChar == '=') {
                        lex[posl++] = '=';
                        lex[posl] = '\0';
                        getNextChar();
                        tokenNumber = TokenNotEqual;
                        return;
                    }

                    // Reconhece o operador de nega��o l�gica.
                    else {
                       lex[posl] = '\0';
                       tokenNumber = TokenLogicalNot;
                       return;
                    }
                }

                // Reconhece os operadores dois pontos e :>.
                if (currentChar == ':') {
                    getNextChar();

                    // Reconhece o operador :>.
                    if (currentChar == '>') {
                        lex[posl++] = '>';
                        lex[posl] = '\0';
                        getNextChar();
                        tokenNumber = TokenCloseBracket;
                        return;
                    }

                    // Reconhece o operador dois pontos.
                    else {
                       lex[posl] = '\0';
                       tokenNumber = TokenColon;
                       return;
                    }
                }

                if (currentChar=='['){lex[posl]='\0'; getNextChar(); tokenNumber=TokenOpenBracket; return;}
                if (currentChar==']'){lex[posl]='\0'; getNextChar(); tokenNumber=TokenCloseBracket; return;}
                if (currentChar=='('){lex[posl]='\0'; getNextChar(); tokenNumber=TokenOpenParentheses; return;}
                if (currentChar==')'){lex[posl]='\0'; getNextChar(); tokenNumber=TokenCloseParentheses; return;}
                if (currentChar=='{'){lex[posl]='\0'; getNextChar(); tokenNumber=TokenOpenBrace; return;}
                if (currentChar=='}'){lex[posl]='\0'; getNextChar(); tokenNumber=TokenCloseBrace; return;}
                if (currentChar==','){lex[posl]='\0'; getNextChar(); tokenNumber=TokenComma; return;}
                if (currentChar==';'){lex[posl]='\0'; getNextChar(); tokenNumber=TokenSemicolon; return;}
                if (currentChar==-1) {lex[posl]='\0'; getNextChar(); tokenNumber=TokenEndOfFile; return;}
                if (currentChar==' ' || currentChar=='\n' || currentChar=='\t' || currentChar=='\r') {getNextChar();posl--;break;}
                if (currentChar=='\0') {tokenNumber=-1;return;}

                printf("ERROR: The lexical analyzer found the character %c (%d) on line %d.\n",currentChar,currentChar,line);
                while (currentChar != '\n') getNextChar();
                break;

            /////////////////////////////////////////////////////////////////

            case 1:
                if ((currentChar>='a' && currentChar<='z') || (currentChar>='A' && currentChar<='Z') || (currentChar == '_') || (currentChar>='0' && currentChar<='9')) {
                    getNextChar();
                    break;
                }

                lex[--posl] = '\0';
                tokenNumber = palavra_reservada(lex);
                return;
          }
       }
}
