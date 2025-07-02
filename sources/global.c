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
int CC71_GlobalCurrentLine = 1;
int CC71_GlobalCurrentColumn = 0;
int columnAux;
char lex[20];
char currentChar;

/////////////////////////////////////////////////////////////////////////////

int topcontexto = 0;
CC71_ContextType pilhacon[1000];

/////////////////////////////////////////////////////////////////////////////

FILE *CC71_GlobalInputFile;
FILE *CC71_GlobalOutputFile;

/////////////////////////////////////////////////////////////////////////////

const int CC71_DebugMode = TRUE;

/////////////////////////////////////////////////////////////////////////////

#define X(name) #name,
const char* tokens[] = {
    TOKEN_LIST
};
#undef X

/////////////////////////////////////////////////////////////////////////////

CC71_KeywordType keywordList[] = {
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
