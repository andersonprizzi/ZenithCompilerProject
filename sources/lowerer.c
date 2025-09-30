#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "lowerer.h"



int zenith_lowerer_debug_line_count(void) { return ir.line_count; }

int zenith_lowerer_debug_writes_to_stdout(void) {
    return ir.output_file == NULL || ir.output_file == stdout;
}

const char* zenith_lowerer_debug_last_line(void) {
    if (ir.line_count <= 0) return "<none>";
    return ir.lines[ir.line_count - 1].text;
}







static void ir_set_place(PlaceType* v, const char* s) {
    strncpy(v->place, s, sizeof(v->place));
    v->place[sizeof(v->place) - 1] = '\0';
}




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
void zenith_lowerer_transaction_begin(void){
    if (ir.transaction_depth < IR_MAX_TRANSACTION_DEPTH) {
        ir.transaction_mark_stack[ir.transaction_depth++] = ir.line_count;
        fprintf(stderr, "[IR] begin depth=%d mark=%d\n", ir.transaction_depth, ir.line_count);
    }
}



/**
 * @brief Descarta tudo que foi emitido desde o último begin().
 *
 * Use isto em sincronia com BacktrackingRestore().
 */
void zenith_lowerer_transaction_abort(void){
    if (ir.transaction_depth > 0) {
        int to = ir.transaction_mark_stack[--ir.transaction_depth];
        fprintf(stderr, "[IR] abort depth=%d from=%d to=%d\n", ir.transaction_depth, ir.line_count, to);
        ir.line_count = to;
    }
}

/**
 * @brief Mantém as linhas emitidas desde o último begin() e fecha a transação.
 *
 * Use isto em sincronia com BacktrackingEnd().
 */
void zenith_lowerer_transaction_commit(void){
    if (ir.transaction_depth > 0) {
        --ir.transaction_depth;
        fprintf(stderr, "[IR] commit depth=%d keep=%d\n", ir.transaction_depth, ir.line_count);
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



void zenith_lowerer_emit_ifz_goto(const char* cond, const char* label) {
    ir_append_line("if %s == 0 goto %s", cond, label);
}

void zenith_lowerer_emit_gototrue(const char* cond, const char* label) {
    ir_append_line("gototrue %s, %s", cond, label);
}

void zenith_lowerer_emit_gt(const char* dst, const char* a, const char* b) {
    ir_append_line("%s := %s > %s", dst, a, b);
}

void zenith_lowerer_emit_lt(const char* dst, const char* a, const char* b) { ir_append_line("%s := %s < %s", dst, a, b); }
void zenith_lowerer_emit_le(const char* dst, const char* a, const char* b) { ir_append_line("%s := %s <= %s", dst, a, b); }
void zenith_lowerer_emit_ge(const char* dst, const char* a, const char* b) { ir_append_line("%s := %s >= %s", dst, a, b); }
void zenith_lowerer_emit_eq(const char* dst, const char* a, const char* b) { ir_append_line("%s := %s == %s", dst, a, b); }
void zenith_lowerer_emit_ne(const char* dst, const char* a, const char* b) { ir_append_line("%s := %s != %s", dst, a, b); }

