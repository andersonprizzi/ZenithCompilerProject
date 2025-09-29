/////////////////////////////////////////////////////////////////////////////
// ABOUT                                                                   //
/////////////////////////////////////////////////////////////////////////////

/**
 * Developed by Anderson Pastore Rizzi.
 * The grammar is described in the file GRAMMAR.md.
 */

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
// COMPLEMENTARY FUNCTIONS                                                 //
/////////////////////////////////////////////////////////////////////////////

int CC71_ValidateFilePointers() {
    if (CC71_GlobalInputFile == NULL || CC71_GlobalOutputErrorFile == NULL) {
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
        fprintf(stderr, "Usage: %s <input_file> <output_error_file\n", argv[0]);
        return 1;
    }
    const char *input_path = argv[1];
    const char *output_error_path = argv[2];

    CC71_GlobalInputFile = fopen(input_path, "r");
    if (!CC71_GlobalInputFile) {
        perror("Error opening input file.");
        return 1;
    }

    CC71_GlobalOutputErrorFile = fopen(output_error_path, "w");
    if (!CC71_GlobalOutputErrorFile) {
        perror("Error opening error output file.");
        fclose(CC71_GlobalOutputErrorFile);
        return 1;
    }

    if (!CC71_ValidateFilePointers()) return 1;

    columnAux = CC71_GlobalCurrentColumn;
    CC71_GetNextChar(); // Reads the first character of the file.
    zenith_get_token();    // Reads the first token.

    zenith_lowerer_open_file("data/output/out_3ac.txt");

    // Lexical-syntactic recognition.
    if (translation_unit()) {
        printf("\nThe source code is syntactically correct.\n\n");
    } else {
        printf("\nThe source code is not syntactically correct.\n\n");
    }

    zenith_lowerer_close_file();

    //char *t1 = zenith_new_label();
    //printf("%s\n", t1);

    // Lexical recognition.
    /*while (CC71_GlobalTokenNumber != TokenEndOfFile) {
        if (OUTPUT_MODE) insertIntoFile(lex, tokens[CC71_GlobalTokenNumber], line, columnAux);
        zenith_get_token();
	}*/

	//if (currentChar == -1) printf("\nThe lexical-syntactic analyzer completed the operations successfully.\n");

	fclose(CC71_GlobalInputFile);
    fflush(CC71_GlobalOutputErrorFile);
	fclose(CC71_GlobalOutputErrorFile);

	return 0;
}
