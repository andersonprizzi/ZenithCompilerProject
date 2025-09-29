#ifndef LOWERER_H_INCLUDED
#define LOWERER_H_INCLUDED

typedef struct {
    char place[64];
} IntermediateRepresentationValue;


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

/* Transações (use com o backtracking) */
void zenith_lowerer_transaction_begin(void);   /* marca um ponto para possíveis descartes */
void zenith_lowerer_transaction_abort(void);   /* desfaz o que foi emitido desde o último begin */
void zenith_lowerer_transaction_commit(void);  /* mantém o que foi emitido desde o último begin */

/* Compatibilidade com nomes antigos (se você já chamou tx_* no projeto) */
void zenith_lowerer_tx_begin(void);
void zenith_lowerer_tx_abort(void);
void zenith_lowerer_tx_commit(void);

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

#endif
