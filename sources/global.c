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
// GLOBAL VARIABLES                                                        //
/////////////////////////////////////////////////////////////////////////////

int CC71_GlobalPosition = 0;
int CC71_GlobalTokenNumber;
int line = 1;
int column = 0;
int columnAux;
char lex[20];
char currentChar;

int topcontexto = 0;
tcontexto pilhacon[1000];

FILE *inputFile;
FILE *outputFile;

/////////////////////////////////////////////////////////////////////////////

const int AC71_DebugMode = TRUE;

/////////////////////////////////////////////////////////////////////////////

#define X(name) #name,
const char* tokens[] = {
    TOKEN_LIST
};
#undef X

/////////////////////////////////////////////////////////////////////////////

Keyword keywordList[] = {
    // PRIMITIVE TYPES //////////////////////////////////////////////////////
    {"void", TokenVoid},
    {"char", TokenChar},
    {"short", TokenShort},
    {"int", TokenInt},
    {"long", TokenLong},
    {"float", TokenFloat},
    {"double", TokenDouble},
    {"_Bool", TokenBool},
    {"_Complex", TokenComplex},

    // TYPE MODIFIERS ///////////////////////////////////////////////////////
    {"signed", TokenSigned},
    {"unsigned", TokenUnsigned},

    // TYPE QUALIFIERS //////////////////////////////////////////////////////
    {"const", TokenConst},
    {"volatile", TokenVolatile},
    {"restrict", TokenRestrict},
    {"_Atomic", TokenAtomic},

    // STORAGE CLASS SPECIFIERS /////////////////////////////////////////////
    {"auto", TokenAuto},
    {"extern", TokenExtern},
    {"register", TokenRegister},
    {"static", TokenStatic},
    {"typedef", TokenTypedef},

    // COMPOSITE TYPES //////////////////////////////////////////////////////
    {"struct", TokenStruct},
    {"union", TokenUnion},
    {"enum", TokenEnum},

    // CONTROL KEYWORDS /////////////////////////////////////////////////////
    {"if", TokenIf},
    {"else", TokenElse},
    {"switch", TokenSwitch},
    {"case", TokenCase},
    {"default", TokenDefault},
    {"while", TokenWhile},
    {"do", TokenDo},
    {"for", TokenFor},
    {"break", TokenBreak},
    {"continue", TokenContinue},
    {"return", TokenReturn},
    {"goto", TokenGoto},
    {"sizeof", TokenSizeof},
    {"inline", TokenInline},

    // IDENTIFIER (Should remain last) //////////////////////////////////////
    {"identifier", TokenIdentifier}
};
