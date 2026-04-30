#include "transaccion.h"
#include "gestor_arch.h"
#include "parser.h"
#include <string.h>
#include <stdlib.h>

BufferTransaccion txn_buffer = {0, 0, {}, 0, "", 0};

void txn_iniciar() {
    if (txn_buffer.activa) {
        out("[WARN] Transaccion ya activa (#%d)\n", txn_buffer.numero);
        return;
    }
    
    FILE *fp = fopen(ARCHIVO_DB, "r");
    txn_buffer.backup_size = 0;
    if (fp) {
        txn_buffer.backup_size = fread(txn_buffer.backup_contenido, 1, sizeof(txn_buffer.backup_contenido) - 1, fp);
        txn_buffer.backup_contenido[txn_buffer.backup_size] = '\0';
        fclose(fp);
    }
    
    txn_buffer.activa = 1;
    txn_buffer.numero++;
    txn_buffer.num_cambios = 0;
    
    out("[TXN] Transaccion #%d iniciada. Backup en memoria.\n", txn_buffer.numero);
}

void txn_agregar_cambio(char op, const char *tabla, const char *clave, int valor) {
    if (!txn_buffer.activa) {
        return;
    }
    
    if (txn_buffer.num_cambios >= MAX_BUFFER) {
        out("[ERROR] Buffer de transacciones lleno.\n");
        return;
    }
    
    Cambio *c = &txn_buffer.cambios[txn_buffer.num_cambios];
    c->operacion = op;
    strncpy(c->tabla, tabla, MAX_CAMPO - 1);
    strncpy(c->clave, clave, MAX_CAMPO - 1);
    c->valor = valor;
    txn_buffer.num_cambios++;
    
    out("[TXN] Cambio registrado en buffer: %c %s:%s\n", op, tabla, clave);
}

int txn_guardar() {
    if (!txn_buffer.activa) {
        out("[WARN] No hay transaccion activa para GUARDAR.\n");
        return 0;
    }
    
    int num = txn_buffer.num_cambios;
    txn_buffer.activa = 0;
    
    for (int i = 0; i < num; i++) {
        Cambio *c = &txn_buffer.cambios[i];
        char val_str[MAX_VALOR];
        snprintf(val_str, sizeof(val_str), "%d", c->valor);
        if (c->operacion == 'A') {
            escribir_registro(c->tabla, c->clave, val_str);
        } else if (c->operacion == 'M') {
            modificar_registro(c->tabla, c->clave, val_str);
        } else if (c->operacion == 'B') {
            borrar_registro(c->tabla, c->clave);
        }
    }
    
    txn_buffer.num_cambios = 0;
    txn_buffer.backup_size = 0;
    
    out("[TXN] Transaccion #%d guardada (COMMIT). %d cambios persistidos.\n", 
        txn_buffer.numero, num);
    txn_buffer.numero++;
    
    return 1;
}

int txn_cancelar() {
    if (!txn_buffer.activa) {
        out("[WARN] No hay transaccion activa para CANCELAR.\n");
        return 0;
    }
    
    FILE *fp = fopen(ARCHIVO_DB, "w");
    if (fp && txn_buffer.backup_size > 0) {
        fwrite(txn_buffer.backup_contenido, 1, txn_buffer.backup_size, fp);
        fclose(fp);
        out("[TXN] Transaccion #%d cancelada (ROLLBACK). Restaurado backup.\n", txn_buffer.numero);
    } else {
        if (fp) fclose(fp);
        fp = fopen(ARCHIVO_DB, "w");
        if (fp) fclose(fp);
        out("[TXN] Transaccion #%d cancelada (ROLLBACK). Archivo vaciado.\n", txn_buffer.numero);
    }
    
    txn_buffer.activa = 0;
    txn_buffer.num_cambios = 0;
    txn_buffer.backup_size = 0;
    txn_buffer.numero++;
    
    return 1;
}

int txn_activa() {
    return txn_buffer.activa;
}