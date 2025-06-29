#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

/////////////////////////////////////////////////////////////////////////////
// LIBRARY INCLUDES                                                        //
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/////////////////////////////////////////////////////////////////////////////
// PREPROCESSING DEFINITIONS                                               //
/////////////////////////////////////////////////////////////////////////////

#define TOKEN_LIST                           \
    /*/// IDENTIFIERS AND CONSTANTS ///////*/\
    X(TokenIdentifier)                       \
    X(TokenIntConst)                         \
    X(TokenFloatConst)                       \
    X(TokenString)                           \
                                             \
    /*/// PRIMITIVE TYPES /////////////////*/\
    X(TokenVoid)                             \
    X(TokenChar)                             \
    X(TokenShort)                            \
    X(TokenInt)                              \
    X(TokenLong)                             \
    X(TokenFloat)                            \
    X(TokenDouble)                           \
    X(TokenBool)                             \
    X(TokenComplex)                          \
                                             \
    /*/// COMPOSITE TYPES /////////////////*/\
    X(TokenStruct)                           \
    X(TokenUnion)                            \
    X(TokenEnum)                             \
                                             \
    /*/// TYPE MODIFIERS //////////////////*/\
    X(TokenSigned)                           \
    X(TokenUnsigned)                         \
                                             \
    /*/// TYPE QUALIFIERS /////////////////*/\
    X(TokenConst)                            \
    X(TokenVolatile)                         \
    X(TokenRestrict)                         \
    X(TokenAtomic)                           \
                                             \
    /*/// STORAGE CLASS SPECIFIERS ////////*/\
    X(TokenAuto)                             \
    X(TokenExtern)                           \
    X(TokenRegister)                         \
    X(TokenStatic)                           \
    X(TokenTypedef)                          \
                                             \
    /*/// ARITHMETIC OPERATORS ////////////*/\
    X(TokenPlus)                             \
    X(TokenMinus)                            \
    X(TokenAsterisk)                         \
    X(TokenDivision)                         \
    X(TokenMod)                              \
    X(TokenIncrement)                        \
    X(TokenDecrement)                        \
                                             \
    /*/// ASSIGNMENT OPERATORS ////////////*/\
    X(TokenAssign)                           \
    X(TokenPlusAssign)                       \
    X(TokenMinusAssign)                      \
    X(TokenMulAssign)                        \
    X(TokenDivAssign)                        \
    X(TokenModAssign)                        \
    X(TokenAndAssign)                        \
    X(TokenOrAssign)                         \
    X(TokenXorAssign)                        \
    X(TokenLeftShiftAssign)                  \
    X(TokenRightShiftAssign)                 \
                                             \
    /*/// RELATIONAL OPERATORS ////////////*/\
    X(TokenEqual)                            \
    X(TokenNotEqual)                         \
    X(TokenGreaterThan)                      \
    X(TokenLessThan)                         \
    X(TokenGreaterEqual)                     \
    X(TokenLessEqual)                        \
                                             \
    /*/// LOGICAL OPERATORS ///////////////*/\
    X(TokenLogicalNot)                       \
    X(TokenLogicalAnd)                       \
    X(TokenLogicalOr)                        \
                                             \
    /*/// BITWISE OPERATORS ///////////////*/\
    X(TokenBitwiseAnd_AddressOf)             \
    X(TokenBitwiseOr)                        \
    X(TokenBitwiseXor)                       \
    X(TokenBitwiseNot)                       \
    X(TokenLeftShift)                        \
    X(TokenRightShift)                       \
                                             \
    /*/// MEMBER ACCESS OPERATORS /////////*/\
    X(TokenDot)                              \
    X(TokenArrow)                            \
                                             \
    /*/// CONDITIONAL OPERATORS ///////////*/\
    X(TokenQuestionMark)                     \
    X(TokenColon)                            \
                                             \
    /*/// DELIMITERS //////////////////////*/\
    X(TokenComma)                            \
    X(TokenSemicolon)                        \
    X(TokenOpenParentheses)                  \
    X(TokenCloseParentheses)                 \
    X(TokenOpenBracket)                      \
    X(TokenCloseBracket)                     \
    X(TokenOpenBrace)                        \
    X(TokenCloseBrace)                       \
                                             \
    /*/// CONTROL KEYWORDS ////////////////*/\
    X(TokenIf)                               \
    X(TokenElse)                             \
    X(TokenSwitch)                           \
    X(TokenCase)                             \
    X(TokenDefault)                          \
    X(TokenWhile)                            \
    X(TokenDo)                               \
    X(TokenFor)                              \
    X(TokenBreak)                            \
    X(TokenContinue)                         \
    X(TokenReturn)                           \
    X(TokenGoto)                             \
    X(TokenSizeof)                           \
    X(TokenInline)                           \
                                             \
    /*/// END OF FILE /////////////////////*/\
    X(TokenEndOfFile)

#define FALSE 0
#define TRUE 1
#define MAX_TOKEN_LENGTH 30



/////////////////////////////////////////////////////////////////////////////
// DEFINITION OF STRUCTURES                                                //
/////////////////////////////////////////////////////////////////////////////

typedef struct keyword{
    char tokenWord[20];
    int tokenNumber;
} Keyword;


typedef struct contexto {
    long posglobal;
    int tkant;
    char cant;
    char lexant[20];
} tcontexto;


#define X(name) name,
enum TokenType {
    TOKEN_LIST
    TokenCount
};
#undef X



/////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES                                                        //
/////////////////////////////////////////////////////////////////////////////


extern char lex[20];

extern int CC71_GlobalPosition;
extern int CC71_GlobalTokenNumber;
extern int CC71_GlobalCurrentLine;
extern int CC71_GlobalCurrentColumn;
extern int columnAux;
extern char currentChar;

extern tcontexto pilhacon[1000];
extern int topcontexto;

extern FILE *CC71_GlobalInputFile;
extern FILE *CC71_GlobalOutputFile;

extern const int CC71_DebugMode;
extern const char* tokens[];
extern Keyword keywordList[];

/////////////////////////////////////////////////////////////////////////////

#endif // GLOBAL_H_INCLUDED
