/////////////////////////////////////////////////////////////////////////////
// LIBRARY INCLUDES                                                        //
/////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "logger.h"
#include "error.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"

/* Largura mínima com zeros à esquerda (T.0000 / L.0000) */
#define ZCP_NAME_PAD_WIDTH 4

/* Contadores internos (um por “espécie” de nome) */
static unsigned zcp_temp_counter  = 0;
static unsigned zcp_label_counter = 0;



/////////////////////////////////////////////////////////////////////////////
// FUNCTION IMPLEMENTATIONS                                                //
/////////////////////////////////////////////////////////////////////////////

/* Função auxiliar para formatar e alocar o nome. */
static char* zcp_alloc_name(const char* prefix, unsigned* counter) {
    size_t need = (size_t)snprintf(NULL, 0, "%s.%0*u", prefix, ZCP_NAME_PAD_WIDTH, *counter) + 1;
    char* out = (char*)malloc(need);

    if (!out) {
        CC71_ReportError(
            CC71_ERR_FATAL_OUT_OF_MEMORY,
            CC71_GlobalCurrentLine,
            columnAux,
            "Out of memory while generating name '%s'.", prefix
        );

        exit(EXIT_FAILURE);
    }

    (void)snprintf(out, need, "%s.%0*u", prefix, ZCP_NAME_PAD_WIDTH, *counter);
    (*counter)++;
    return out;
}

char* zenith_new_temp(void) {
    return zcp_alloc_name("T", &zcp_temp_counter);
}

char* zenith_new_label(void) {
    return zcp_alloc_name("L", &zcp_label_counter);
}
