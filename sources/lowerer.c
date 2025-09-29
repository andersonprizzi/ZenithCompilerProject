#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "lowerer.h"



enum {
    IR_MAX_LINES             = 200000,  /* nº máx. de linhas TAC em memória */
    IR_MAX_LINE_LENGTH       = 256,     /* tamanho máx. de uma linha TAC    */
    IR_MAX_TRANSACTION_DEPTH = 512      /* profundidade máx. begin/abort/commit */
};








/** Uma linha de TAC (texto já formatado) */
typedef struct {
    char text[IR_MAX_LINE_LENGTH];
} IRLine;

/** Estado global do emissor */
typedef struct {
    /* Buffer de linhas de TAC acumuladas em memória */
    IRLine lines[IR_MAX_LINES];
    int    line_count;

    /* Pilha de marcas de transação (cada marca guarda o line_count de início) */
    int    transaction_mark_stack[IR_MAX_TRANSACTION_DEPTH];
    int    transaction_depth;

    /* Saída em arquivo */
    FILE*  output_file;
    char   output_path[260]; /* caminho opcional; 260 cobre o comum no Windows */
} IRState;

static IRState ir; /* estado único deste módulo (escopo de arquivo) */








/**
 * @brief Acrescenta uma linha formatada ao buffer de IR.
 * @note Linhas não incluem '\n' aqui; adicionamos no dump.
 */
static void ir_append_line(const char* format, ...) {
    if (ir.line_count >= IR_MAX_LINES) {
        /* Se quiser, troque por realocação dinâmica + abort se falhar */
        return;
    }

    va_list args;
    va_start(args, format);
    vsnprintf(ir.lines[ir.line_count].text, IR_MAX_LINE_LENGTH, format, args);
    va_end(args);

    ir.line_count++;
}

/** @brief Grava o buffer acumulado no arquivo de saída. */
static void ir_dump_to_file(FILE* f) {
    for (int i = 0; i < ir.line_count; ++i) {
        fputs(ir.lines[i].text, f);
        fputc('\n', f);
    }
}

/** @brief Reinicia o estado (linhas e transações), mantendo o arquivo fechado. */
static void ir_reset_memory(void) {
    ir.line_count        = 0;
    ir.transaction_depth = 0;
}









void zenith_lowerer_open_file(const char* path) {
    /* Fecha anterior, se necessário */
    if (ir.output_file && ir.output_file != stdout) {
        fclose(ir.output_file);
        ir.output_file = NULL;
    }

    /* Guarda caminho e abre (se path vier NULL, use stdout como fallback) */
    if (path && *path) {
        strncpy(ir.output_path, path, sizeof(ir.output_path) - 1);
        ir.output_path[sizeof(ir.output_path) - 1] = '\0';
        ir.output_file = fopen(ir.output_path, "w");
        if (!ir.output_file) {
            perror("zenith_lowerer_open_file");
            ir.output_file = stdout;
        }
    } else {
        ir.output_path[0] = '\0';
        ir.output_file    = stdout;
    }

    /* Limpamos o buffer de IR para iniciar uma nova compilação */
    ir_reset_memory();
}

void zenith_lowerer_close_file(void) {
    /* Se por alguma razão ninguém chamou open_file, default para stdout */
    if (!ir.output_file) {
        ir.output_file = stdout;
    }

    /* Despeja o IR acumulado */
    ir_dump_to_file(ir.output_file);

    /* Fecha se não for stdout */
    if (ir.output_file != stdout) {
        fclose(ir.output_file);
    }

    ir.output_file = NULL;
    ir.output_path[0] = '\0';

    /* Opcional: manter IR em memória para debug; aqui zeramos */
    ir_reset_memory();
}











/**
 * @brief Inicia uma transação de emissão.
 * 
 * Tudo que for emitido após este ponto será mantido **apenas** se você
 * chamar zenith_lowerer_transaction_commit(). Caso chame
 * zenith_lowerer_transaction_abort(), todo esse trecho será descartado.
 *
 * Use isto em sincronia com BacktrackingStart().
 */
void zenith_lowerer_transaction_begin(void) {
    if (ir.transaction_depth < IR_MAX_TRANSACTION_DEPTH) {
        ir.transaction_mark_stack[ir.transaction_depth++] = ir.line_count;
    }
}

/**
 * @brief Descarta tudo que foi emitido desde o último begin().
 *
 * Use isto em sincronia com BacktrackingRestore().
 */
void zenith_lowerer_transaction_abort(void) {
    if (ir.transaction_depth > 0) {
        ir.line_count = ir.transaction_mark_stack[--ir.transaction_depth];
    }
}

/**
 * @brief Mantém as linhas emitidas desde o último begin() e fecha a transação.
 *
 * Use isto em sincronia com BacktrackingEnd().
 */
void zenith_lowerer_transaction_commit(void) {
    if (ir.transaction_depth > 0) {
        --ir.transaction_depth; /* nada a fazer: as linhas já estão no buffer */
    }
}





// Emissão de c. 3-endereços

void zenith_lowerer_emit_assign(const char* dst, const char* src) {
    ir_append_line("%s := %s", dst, src);
}

void zenith_lowerer_emit_add(const char* dst, const char* a, const char* b) {
    ir_append_line("%s := %s + %s", dst, a, b);
}

void zenith_lowerer_emit_sub(const char* dst, const char* a, const char* b) {
    ir_append_line("%s := %s - %s", dst, a, b);
}

void zenith_lowerer_emit_mul(const char* dst, const char* a, const char* b) {
    ir_append_line("%s := %s * %s", dst, a, b);
}

void zenith_lowerer_emit_div(const char* dst, const char* a, const char* b) {
    ir_append_line("%s := %s / %s", dst, a, b);
}

void zenith_lowerer_emit_mod(const char* dst, const char* a, const char* b) {
    ir_append_line("%s := %s %% %s", dst, a, b);
}

void zenith_lowerer_emit_label(const char* label) {
    ir_append_line("%s:", label);
}

void zenith_lowerer_emit_goto(const char* label) {
    ir_append_line("goto %s", label);
}

void zenith_lowerer_emit_ifnz_goto(const char* cond, const char* label) {
    ir_append_line("if %s != 0 goto %s", cond, label);
}
