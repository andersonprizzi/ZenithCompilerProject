#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "lowerer.h"



/* Detectores auxiliares */
static int is_number(const char* s){
    if(!s||!*s) return 0;
    if(*s=='+'||*s=='-') s++;
    if(!*s) return 0;
    while(*s){ if(*s<'0'||*s>'9') return 0; s++; }
    return 1;
}
static int is_label_name(const char* s){ return s && s[0]=='L' && s[1]=='.'; }
static int is_temp_name (const char* s){ return s && s[0]=='T' && s[1]=='.'; }

/* identificador simples (aceita letra/_ no início) */
static int is_identifier(const char* s){
    if(!s||!*s) return 0;
    if(!((s[0]>='A'&&s[0]<='Z')||(s[0]>='a'&&s[0]<='z')||s[0]=='_')) return 0;
    return 1;
}


/* Insere se ainda não existir; ignora números e labels */
static void ir_register_var(const char* name){
    if(!name||!*name) return;
    if(is_number(name) || is_label_name(name)) return;
    for(int i=0;i<var_count;i++) if(strcmp(var_names[i],name)==0) return;
    if(var_count<IR_MAX_VARS){
        strncpy(var_names[var_count], name, IR_MAX_NAME-1);
        var_names[var_count][IR_MAX_NAME-1]='\0';
        var_is_temp[var_count] = is_temp_name(name) ? 1 : 0;
        var_count++;
    }
}


/* Registra “places” (dst/operando) que não são números/labels */
static void ir_track_place(const char* p){
    if(!p) return;
    if(is_number(p) || is_label_name(p)) return;
    if(is_temp_name(p) || is_identifier(p)) ir_register_var(p);
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
    if (ir.line_count >= IR_MAX_LINES) return;

    /* Formata em buf */
    char buf[IR_MAX_LINE_LENGTH];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    /* Classifica a linha */
    const char* s = buf;
    while (*s == ' ' || *s == '\t') s++;      /* trim left simples */

    size_t len = strlen(s);
    int is_empty = (len == 0);

    /* "label" = termina com ':' e não tem espaços no conteúdo */
    int is_label = 0;
    if (!is_empty) {
        size_t end = len;
        while (end > 0 && (s[end - 1] == ' ' || s[end - 1] == '\t')) end--; /* trim right */
        if (end > 0 && s[end - 1] == ':') {
            int has_space = 0;
            for (size_t k = 0; k + 1 < end; ++k) {
                if (s[k] == ' ' || s[k] == '\t') { has_space = 1; break; }
            }
            if (!has_space) is_label = 1;
        }
    }

    /* Se é label, insere linha em branco antes (se a última não for vazia) */
    if (is_label) {
        if (ir.line_count > 0) {
            const char* last = ir.lines[ir.line_count - 1].text;
            if (last[0] != '\0') {
                ir.lines[ir.line_count].text[0] = '\0';  /* linha vazia */
                ir.line_count++;
            }
        }
        /* grava label SEM indentação */
        strncpy(ir.lines[ir.line_count].text, s, IR_MAX_LINE_LENGTH - 1);
        ir.lines[ir.line_count].text[IR_MAX_LINE_LENGTH - 1] = '\0';
        ir.line_count++;

        /* ativa indentação para as próximas instruções */
        ir_indent_active = 1;
        return;
    }

    /* Instruções: indenta se estamos após uma label e a linha não é vazia */
    if (ir_indent_active && !is_empty) {
        size_t ind_len  = strlen(IR_INDENT);
        size_t copy_len = strlen(s);
        if (ind_len + copy_len >= IR_MAX_LINE_LENGTH) {
            copy_len = IR_MAX_LINE_LENGTH - ind_len - 1;
        }
        memcpy(ir.lines[ir.line_count].text, IR_INDENT, ind_len);
        memcpy(ir.lines[ir.line_count].text + ind_len, s, copy_len);
        ir.lines[ir.line_count].text[ind_len + copy_len] = '\0';
    } else {
        strncpy(ir.lines[ir.line_count].text, s, IR_MAX_LINE_LENGTH - 1);
        ir.lines[ir.line_count].text[IR_MAX_LINE_LENGTH - 1] = '\0';
    }

    ir.line_count++;
}


/** @brief Grava o buffer acumulado no arquivo de saída. */
static void ir_dump_to_file(FILE* f) {
    // Cabeçalho de declarações:
    // primeiro identificadores do usuário
    // segundo temporárias T.xxxx
    for (int pass = 0; pass < 2; ++pass) {
        for (int i = 0; i < var_count; ++i) {
            if ((pass == 0 && !var_is_temp[i]) || (pass == 1 && var_is_temp[i])) {
                fprintf(f, "int %s\n", var_names[i]);
            }
        }
    }

    if (var_count > 0) fputc('\n', f);  // linha em branco após as declarações.

    // Corpo de instruções de 3 endereços
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
        //fprintf(stderr, "[IR] begin depth=%d mark=%d\n", ir.transaction_depth, ir.line_count);
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
        //fprintf(stderr, "[IR] abort depth=%d from=%d to=%d\n", ir.transaction_depth, ir.line_count, to);
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
        //fprintf(stderr, "[IR] commit depth=%d keep=%d\n", ir.transaction_depth, ir.line_count);
    }
}



// Emissão de C3E

void zenith_lowerer_emit_assign(const char* dst, const char* src) {
    ir_track_place(dst); ir_track_place(src);
    ir_append_line("%s := %s", dst, src);
}
void zenith_lowerer_emit_add(const char* dst, const char* a, const char* b) {
    ir_track_place(dst); ir_track_place(a); ir_track_place(b);
    ir_append_line("%s := %s + %s", dst, a, b);
}
void zenith_lowerer_emit_sub(const char* dst, const char* a, const char* b) {
    ir_track_place(dst); ir_track_place(a); ir_track_place(b);
    ir_append_line("%s := %s - %s", dst, a, b);
}
void zenith_lowerer_emit_mul(const char* dst, const char* a, const char* b) {
    ir_track_place(dst); ir_track_place(a); ir_track_place(b);
    ir_append_line("%s := %s * %s", dst, a, b);
}
void zenith_lowerer_emit_div(const char* dst, const char* a, const char* b) {
    ir_track_place(dst); ir_track_place(a); ir_track_place(b);
    ir_append_line("%s := %s / %s", dst, a, b);
}
void zenith_lowerer_emit_mod(const char* dst, const char* a, const char* b) {
    ir_track_place(dst); ir_track_place(a); ir_track_place(b);
    ir_append_line("%s := %s %% %s", dst, a, b);
}

void zenith_lowerer_emit_label(const char* label) {
    ir_append_line("%s:", label); /* labels não são variáveis */
}
void zenith_lowerer_emit_goto(const char* label) {
    ir_append_line("goto %s", label);
}
void zenith_lowerer_emit_ifnz_goto(const char* cond, const char* label) {
    ir_track_place(cond);
    ir_append_line("if %s != 0 goto %s", cond, label);
}
void zenith_lowerer_emit_ifz_goto(const char* cond, const char* label) {
    ir_track_place(cond);
    ir_append_line("if %s == 0 goto %s", cond, label);
}
void zenith_lowerer_emit_gototrue(const char* cond, const char* label) {
    ir_track_place(cond);
    ir_append_line("gototrue %s, %s", cond, label);
}

/* Comparações */
void zenith_lowerer_emit_gt(const char* dst, const char* a, const char* b) {
    ir_track_place(dst); ir_track_place(a); ir_track_place(b);
    ir_append_line("%s := %s > %s", dst, a, b);
}
void zenith_lowerer_emit_lt(const char* dst, const char* a, const char* b) {
    ir_track_place(dst); ir_track_place(a); ir_track_place(b);
    ir_append_line("%s := %s < %s", dst, a, b);
}
void zenith_lowerer_emit_le(const char* dst, const char* a, const char* b) {
    ir_track_place(dst); ir_track_place(a); ir_track_place(b);
    ir_append_line("%s := %s <= %s", dst, a, b);
}
void zenith_lowerer_emit_ge(const char* dst, const char* a, const char* b) {
    ir_track_place(dst); ir_track_place(a); ir_track_place(b);
    ir_append_line("%s := %s >= %s", dst, a, b);
}
void zenith_lowerer_emit_eq(const char* dst, const char* a, const char* b) {
    ir_track_place(dst); ir_track_place(a); ir_track_place(b);
    ir_append_line("%s := %s == %s", dst, a, b);
}
void zenith_lowerer_emit_ne(const char* dst, const char* a, const char* b) {
    ir_track_place(dst); ir_track_place(a); ir_track_place(b);
    ir_append_line("%s := %s != %s", dst, a, b);
}
