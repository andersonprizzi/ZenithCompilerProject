#ifndef LOWERER_H_INCLUDED
#define LOWERER_H_INCLUDED


// Formatting state
static int ir_indent_active = 0;
#ifndef IR_INDENT
    #define IR_INDENT "\t"
#endif

// Registro de variáveis para cabeçalho de declarações
#ifndef IR_MAX_VARS
    #define IR_MAX_VARS 2048
#endif
#ifndef IR_MAX_NAME
    #define IR_MAX_NAME 128
#endif

// Captura de place.
#ifndef PLACE_CAPTURE_STACK_MAX
    #define PLACE_CAPTURE_STACK_MAX 128
#endif



enum {
    IR_MAX_LINES             = 200000,  /* nº máx. de linhas TAC em memória */
    IR_MAX_LINE_LENGTH       = 256,     /* tamanho máx. de uma linha TAC    */
    IR_MAX_TRANSACTION_DEPTH = 512      /* profundidade máx. begin/abort/commit */
};



/** Uma linha de C3E */
typedef struct {
    char text[IR_MAX_LINE_LENGTH];
} IRLine;



typedef struct {
    char place[64];
} PlaceType;


static PlaceType* g_place_capture_stack[PLACE_CAPTURE_STACK_MAX];
static int g_place_capture_depth = 0;


static char var_names[IR_MAX_VARS][IR_MAX_NAME];
static int  var_is_temp[IR_MAX_VARS];
static int  var_count = 0;



/** Estado global do emissor */
typedef struct {
    /* Buffer de linhas de TAC acumuladas em memória */
    IRLine lines[IR_MAX_LINES];
    int line_count;

    /* Pilha de marcas de transação (cada marca guarda o line_count de início) */
    int transaction_mark_stack[IR_MAX_TRANSACTION_DEPTH];
    int transaction_depth;

    /* Saída em arquivo */
    FILE* output_file;
    char output_path[260]; /* caminho opcional; 260 cobre o comum no Windows */
} IRState;

static IRState ir;



/**
 * Emissor de código intermediário (3 endereços) com transações
 *
 * Conceito:
 *  - Todo TAC é acumulado num buffer em memória.
 *  - Transações permitem tentar caminhos da gramática; se falhar, descarta.
 *  - Ao final, tudo que permaneceu é gravado no arquivo.
 *
 * Integração com backtracking:
 *  - BacktrackingStart()  -> zenith_lowerer_transaction_begin()
 *  - BacktrackingRestore()-> zenith_lowerer_transaction_abort()
 *  - BacktrackingEnd()    -> zenith_lowerer_transaction_commit()
 */

/* Ciclo de vida do arquivo de saída */
void zenith_lowerer_open_file(const char* path); /* abre/define o destino (ex.: "out_3ac.txt") */
void zenith_lowerer_close_file(void);            /* despeja o buffer atual no arquivo e fecha */


static void ir_set_place(PlaceType* v, const char* s);



/* Transações (use com o backtracking) */
void zenith_lowerer_transaction_begin(void);   /* marca um ponto para possíveis descartes */
void zenith_lowerer_transaction_abort(void);   /* desfaz o que foi emitido desde o último begin */
void zenith_lowerer_transaction_commit(void);  /* mantém o que foi emitido desde o último begin */

/* Compatibilidade com nomes antigos (se você já chamou tx_* no projeto) */
void zenith_lowerer_transaction_begin(void);
void zenith_lowerer_transaction_abort(void);
void zenith_lowerer_transaction_commit(void);

/* Emissão de código intermediário. */
void zenith_lowerer_emit_assign(const char* dst, const char* src);
void zenith_lowerer_emit_add   (const char* dst, const char* a, const char* b);
void zenith_lowerer_emit_sub   (const char* dst, const char* a, const char* b);
void zenith_lowerer_emit_mul   (const char* dst, const char* a, const char* b);
void zenith_lowerer_emit_div   (const char* dst, const char* a, const char* b);
void zenith_lowerer_emit_mod   (const char* dst, const char* a, const char* b);

/* Controle de fluxo (úteis para if/while/for numa próxima etapa) */
void zenith_lowerer_emit_label (const char* label);                 /* label: */
void zenith_lowerer_emit_goto  (const char* label);                 /* goto L */
void zenith_lowerer_emit_ifnz_goto(const char* cond,const char* L); /* if cond != 0 goto L */
void zenith_lowerer_emit_gt(const char* dst, const char* a, const char* b);
void zenith_lowerer_emit_ifz_goto(const char* cond, const char* label);

void zenith_lowerer_emit_gototrue(const char* cond, const char* label);

void zenith_lowerer_emit_lt(const char* dst, const char* a, const char* b);
void zenith_lowerer_emit_le(const char* dst, const char* a, const char* b);
void zenith_lowerer_emit_ge(const char* dst, const char* a, const char* b);
void zenith_lowerer_emit_eq(const char* dst, const char* a, const char* b);
void zenith_lowerer_emit_ne(const char* dst, const char* a, const char* b);




/* Inicia a captura do próximo "place".
   Retorna 1 em sucesso, 0 se 'out' for NULL ou se a pilha estiver cheia. */
static int PlaceCaptureBegin(PlaceType* out) {
    if (!out) return 0;
    if (g_place_capture_depth >= PLACE_CAPTURE_STACK_MAX) return 0;
    g_place_capture_stack[g_place_capture_depth++] = out;
    return 1;
}

/* Encerra a captura iniciada por PlaceCaptureBegin().
   Se não houver captura ativa, simplesmente não faz nada. */
static void PlaceCaptureEnd(void) {
    if (g_place_capture_depth > 0) {
        --g_place_capture_depth;
    }
}




static PlaceType g_last_place;
static int       g_last_place_valid = 0;



/* Entrega um "place" (nome) para quem estiver capturando, se houver alguém. */
/*static void PlaceCaptureProvide(const char* place_name)
{
    if (g_place_capture_depth == 0 || !place_name) return;
    PlaceType* dst = g_place_capture_stack[g_place_capture_depth - 1];
    strncpy(dst->place, place_name, sizeof dst->place);
    dst->place[sizeof dst->place - 1] = '\0';
}*/


/* Entrega um "place" (nome) para quem estiver capturando, e também
   registra como "último place" visto para quem precisar (ex.: sufixo ++/--) */
static void PlaceCaptureProvide(const char* place_name)
{
    if (!place_name) return;

    // Se alguém está capturando, escreve lá
    if (g_place_capture_depth > 0) {
        PlaceType* dst = g_place_capture_stack[g_place_capture_depth - 1];
        strncpy(dst->place, place_name, sizeof dst->place);
        dst->place[sizeof dst->place - 1] = '\0';
    }

    // Sempre registra o último place produzido
    strncpy(g_last_place.place, place_name, sizeof g_last_place.place);
    g_last_place.place[sizeof g_last_place.place - 1] = '\0';
    g_last_place_valid = 1;
}


/* Retorna 1 se 's' parece um identificador simples de C (ASCII), 0 caso contrário. */
static int PlaceIsSimpleIdentifier(const char* s)
{
    if (!s || !*s) return 0;

    unsigned char c = (unsigned char)s[0];
    if (!(c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) return 0;

    for (const unsigned char* p = (const unsigned char*)s + 1; *p; ++p) {
        unsigned char ch = *p;
        if (!(ch == '_' || (ch >= 'A' && ch <= 'Z') ||
              (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9'))) {
            return 0;
        }
    }
    return 1;
}

#endif
