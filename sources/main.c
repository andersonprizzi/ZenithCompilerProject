/////////////////////////////////////////////////////////////////////////////
// ABOUT                                                                   //
/////////////////////////////////////////////////////////////////////////////

/**
 * Developed by Anderson Pastore Rizzi.
 * Based on ANSI C Yacc Grammar.
 */

/////////////////////////////////////////////////////////////////////////////
// LIBRARY INCLUDES                                                        //
/////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "logger.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"

/////////////////////////////////////////////////////////////////////////////
// COMPLEMENTARY FUNCTIONS                                                 //
/////////////////////////////////////////////////////////////////////////////

/*
void insertIntoFile(const char *lexeme, const char *tokenName, int lineNumber, int columnNumber) {
    fprintf(outputFile, "Token '%s' (%s) was found at line %d, column %d.\n", lexeme, tokenName, lineNumber, columnNumber);
}
*/


void AC71_OpenSourceFile(const char *inputFileName, const char *outputFileName) {
    inputFile = fopen(inputFileName,"rb");
    outputFile = fopen(outputFileName, "w");
}


int AC71_ValidateFilePointers() {
    if (inputFile == NULL || outputFile == NULL) {
        printf("ERROR: Could not open file.\n");
        return FALSE;
    }
    return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION                                                           //
/////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[2];

    inputFile = fopen(input_path, "r");
    if (!inputFile) {
        perror("Error opening input file.");
        return 1;
    }

    outputFile = fopen(output_path, "w");
    if (!outputFile) {
        perror("Error opening output file.");
        fclose(inputFile);
        return 1;
    }

    //AC71_OpenSourceFile("font.c","output.txt");
    if (!AC71_ValidateFilePointers()) return 1;

    columnAux = column;
    getNextChar(); // Reads the first character of the file.
    getToken();    // Reads the first token.

    // Lexical-syntactic recognition.
    //if (translation_unit()) {
    if (statement()) {
        printf("Reconheceu o codigo-fonte.\n");
    } else {
        printf("Nao reconheceu o codigo-fonte.\n");
    }

    /*while (tokenNumber != TokenEndOfFile) {
        if (OUTPUT_MODE) insertIntoFile(lex, tokens[tokenNumber], line, columnAux);
        getToken();
	}*/

	if (currentChar == -1) printf("The lexical-syntactic analyzer completed the operations successfully.\n");

	fclose(inputFile);
	fclose(outputFile);

	return 0;
}
